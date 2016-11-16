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

class vsx_thread_pool
{

public:

  enum priority {
    low_priority,
    normal_priority,
    high_priority
  };

  explicit vsx_thread_pool(size_t threads = std::thread::hardware_concurrency())
    :
    tasks(
      [](prioritized_task l, prioritized_task r){ return std::get<0>(l) < std::get<0>(r); }
    )
  {
    tasks_queued = 0;

    for_n (i, 0, threads)
      workers.emplace_back(
        [this]
        {
          forever
          {
            std::function<void()> task;

            {
              std::unique_lock<std::mutex> lock(this->queue_mutex);

              // waits if not stopping
              // waits if tasks empty
              this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

              if(this->stop && this->tasks.empty())
                return;

              task = std::get<1>(std::move(this->tasks.top()));
              this->tasks.pop();
            }

            task();
            this->tasks_queued--;
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
  inline auto add(vsx_thread_pool::priority p, F&& f, Args&&... args)
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

      tasks.emplace(prioritized_task( static_cast<int>(p), [task](){ (*task)(); } ));
    }
    condition.notify_one();
    return res;
  }

  // Add tasks to the task queue
  template<class F, class... Args>
  inline auto add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
  {
    return add(normal_priority, f, args...);
  }

  inline bool is_jobless()
  {
    return tasks.empty();
  }

  inline bool wait_all(size_t milliseconds)
  {
    foreach(workers, i)
      if (std::this_thread::get_id() == workers[i].get_id())
      {
        vsx_printf(L"\n\n\n\nWARNING!!! DO NOT RUN THREAD POOL WAIT ALL INSIDE A THREAD POOL TASK!!!\n      If the pool is filled, you end up with a deadlock. \n\n");
        return false;
      }

    while (tasks_queued.load())
      std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

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
  using prioritized_task = std::tuple< uint64_t, std::function<void()> >;
  std::priority_queue< prioritized_task,
                       std::vector<prioritized_task>,
                       std::function<bool(prioritized_task, prioritized_task)> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop = false;

  // More synchronization for wait_all()
  std::atomic<uint64_t> tasks_queued;
  std::mutex queue_empty_mutex;
  std::condition_variable queue_empty_condition;
};


#define threaded_task vsx_thread_pool::instance()->add( [&]()
#define threaded_task_end );
#define threaded_task_wait_all(ms) vsx_thread_pool::instance()->wait_all(ms)
