#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class thread_pool {
public:
  //TODO: add a sleep timer, after which threads will be killed if inactive
  explicit thread_pool(size_t = std::thread::hardware_concurrency());
  ~thread_pool();
  
  // Add tasks to the task queue
  template<class F, class... Args>
  auto enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>;

  // Are there no more tasks to process?
  bool is_jobless();

  // Wait till the tasks are processed
  void wait_all();

  //global thread pool instance
  static thread_pool* instance()
  {
    static thread_pool p;
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
  bool stop;

  // More synchronization for wait_all()
  std::mutex queue_empty_mutex;
  std::condition_variable queue_empty_condition;
};
 
// the constructor just launches some amount of workers
inline thread_pool::thread_pool(size_t threads)
  :   stop(false)
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
            this->condition.wait(lock,
              [this]{ return this->stop || !this->tasks.empty(); });
            if(this->stop && this->tasks.empty())
              return;
            task = std::move(this->tasks.front());
            this->tasks.pop();
          }

          task();

          if (this->tasks.empty())
            this->queue_empty_condition.notify_all();
        }
      }
    );
}

// the destructor joins all threads
inline thread_pool::~thread_pool()
{
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for(std::thread &worker: workers)
    worker.join();
}


// add new work item to the pool
template<class F, class... Args>
auto thread_pool::enqueue(F&& f, Args&&... args) 
  -> std::future<typename std::result_of<F(Args...)>::type>
{
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


inline bool thread_pool::is_jobless()
{
  return tasks.empty();
}


inline void thread_pool::wait_all()
{
  if (tasks.empty())
    return;

  std::unique_lock<std::mutex> queue_empty_lock(this->queue_empty_mutex);
  queue_empty_condition.wait(queue_empty_lock);
}
#endif
