#pragma once                                                                                                  
                                                                                                              
#include <deque>                                                                                              
#include <thread>                                                                                             
#include <memory>                                                                                             
#include <atomic>                                                                                             
#include <mutex>                                                                                              
#include <condition_variable>                                                                                 
#include <vector>                                                                                             
#include <functional>
#include <future>
#include "Work.h"

class ThreadPool {
  public:
  void start(unsigned int numberOfThreads) {
      for(unsigned int i{0}; i < numberOfThreads; i++) {
         threads.push_back(std::make_shared<std::thread>(&ThreadPool::worker, this));
      } 
  }

  void worker() {
     while (!done) {
         Work work;
         {
             std::unique_lock<std::mutex> ul(m);
             cv.wait(ul,[&] { return ( hasWork() || done );});
             work = pull();
         }

         if(done) break;
         work.execute(); 
     }
  }

  void add(Work work) {
     {
         std::lock_guard<std::mutex> guard(m); 
         workQueue.push_front(work);
     }
     cv.notify_one();
  }
  
  template<class F>
    auto submit(F&& task_function) 
    {
        using T = decltype(task_function());

        auto task = std::make_shared<std::packaged_task<T()>> (
            std::bind(std::forward<F>(task_function))            
        );
        
        std::future<T> result = task->get_future();
        auto lambdaf = [task] () { (*task)(); };
        Work work(lambdaf);

        {
            std::lock_guard<std::mutex> guard(m);
            workQueue.push_front(work);
        }

        cv.notify_one();

        return result;
    }

  bool hasWork() {
     return !workQueue.empty();
  }

  Work pull() {
     if(workQueue.empty()) {
         return Work{};
     }

     auto work = workQueue.back();
     workQueue.pop_back();
     return work;
  }

  ~ThreadPool() {
     done = true; 
     cv.notify_all();
     for(auto& t:threads) {
         t->join();
     }
  }

  std::deque<Work> workQueue;                                                          
  std::atomic<bool> done {false};                                                                       
  std::mutex m;                                                                                         
  std::condition_variable cv;                                                                           
  std::vector<std::shared_ptr<std::thread>> threads;  
};
