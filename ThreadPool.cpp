#include "ThreadPool.h"

void ThreadPool::start(unsigned int numberOfThreads) {
    for(unsigned int i{0}; i < numberOfThreads; i++) {
        threads.push_back(std::make_shared<std::thread>(&ThreadPool::worker, this));
    } 
}

void ThreadPool::worker() {
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

void ThreadPool::add(Work work) {
    {
        std::lock_guard<std::mutex> guard(m); 
        workQueue.push_front(work);
    }
    cv.notify_one();
}

bool ThreadPool::hasWork() {
    return !workQueue.empty();
}

Work ThreadPool::pull() {
    if(workQueue.empty()) return Work{};
    auto work = workQueue.back();
    workQueue.pop_back();
    return work;
}

ThreadPool::~ThreadPool() {
    done = true; 
    cv.notify_all();
    for(auto& t:threads) {
        t->join();
    }
}
