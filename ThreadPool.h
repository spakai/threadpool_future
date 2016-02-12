#pragma once

#include <deque>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "Work.h"

class ThreadPool {
    private:
        std::deque<Work> workQueue;
        std::atomic<bool> done {false};    
        std::mutex m;
        std::condition_variable cv;
        std::vector<std::shared_ptr<std::thread>> threads;
    public:
        ~ThreadPool();
        void start(unsigned int numberOfThreads);
        void worker();
        virtual void add(Work work);
        bool hasWork();
        Work pull();
};
