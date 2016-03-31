#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include <vsx_platform.h>
#include <tools/vsx_foreach.h>
#include <string/vsx_printf.h>
#include <debug/vsx_backtrace.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <platform/win64/mingw-std-threads/mingw.thread.h>
  #include <platform/win64/mingw-std-threads/mingw.mutex.h>
  #include <platform/win64/mingw-std-threads/mingw.condition_variable.h>
#endif

class vsx_thread_pool
{
public:

  explicit vsx_thread_pool(size_t threads = std::thread::hardware_concurrency())
  {
    for(size_t i = 0;i<threads;++i)
      workers.emplace_back(
        [this]
        {
          for(;;)
          {
            std::function<void()> task;

            {
              std::unique_lock<std::mutex> lock(this->queue_mutex);

              this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

              if(this->stop && this->tasks.empty())
                return;

              task = std::move(this->tasks.front());
              this->tasks.pop();
            }

            task();
            this->tasks_queued--;

            if (!this->tasks_queued.load())
              this->queue_empty_condition.notify_all();
          }
        }
      );
  }

  ~vsx_thread_pool()
  {
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
      worker.join();
  }

  // Add tasks to the task queue
  template<class F, class... Args>
  inline auto add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
  {
    tasks_queued++;

    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
      );

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queue_mutex);

      // don't allow enqueueing after stopping the pool
      if(stop)
        throw std::runtime_error("enqueue on stopped thread_pool");

      tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
  }

  inline bool is_jobless()
  {
    return tasks.empty();
  }

  inline bool wait_all()
  {
    foreach(workers, i)
      if (std::this_thread::get_id() == workers[i].get_id())
        vsx_printf(L"\n\n\n\nWARNING!!! DO NOT RUN THREAD POOL WAIT ALL INSIDE A THREAD POOL TASK!!!\n      If the pool is filled, you end up with a deadlock. \n\n");
        return false;

    if ( !tasks_queued.load() )
      return true;

    std::unique_lock<std::mutex> queue_empty_lock(this->queue_empty_mutex);
    queue_empty_condition.wait(queue_empty_lock);
    return true;
  }

  static vsx_thread_pool* instance()
  {
    static vsx_thread_pool p;
    return &p;
  }

private:

  // need to keep track of threads so we can join them
  std::vector< std::thread > workers;
  // the task queue
  std::queue< std::function<void()> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop = false;

  // More synchronization for wait_all()
  std::atomic<int> tasks_queued;
  std::mutex queue_empty_mutex;
  std::condition_variable queue_empty_condition;
};


#define threaded_task vsx_thread_pool::instance()->add( [&]()
#define threaded_task_end );
#define threaded_task_wait_all vsx_thread_pool::instance()->wait_all()
