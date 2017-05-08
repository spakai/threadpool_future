#include "gmock/gmock.h" 
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <set>
#include <vector>

#include "ThreadPool.h"
#include "TestTimer.h"

using namespace testing;

class ThreadPoolTest : public Test {
    public:
        ThreadPool pool;
        std::condition_variable wasExecuted;
        std::mutex m;
        std::vector<std::shared_ptr<std::thread>> threads; 
 
        unsigned int count{0};
 
        void incrementCountAndNotify() {
            std::unique_lock<std::mutex> lock(m);
            ++count;
            wasExecuted.notify_all();
        }
       
        void waitForNotificationOrFailOnTimeout(unsigned expectedCount, int milliseconds=100) {
            std::unique_lock<std::mutex> lock(m);
            ASSERT_THAT(wasExecuted.wait_for(lock, std::chrono::milliseconds(milliseconds), [&] { return count == expectedCount; }), Eq(true));      
 
        } 

        void TearDown() override {
            for (auto& t: threads) t->join();
        }
};

TEST_F(ThreadPoolTest,PoolHasWorkAfterAdd) {
    pool.add([]{});
    ASSERT_THAT(pool.hasWork(), Eq(1));
}

TEST_F(ThreadPoolTest,PoolHasNoWorkAfterCreation) {
    ASSERT_THAT(pool.hasWork(), Eq(0));
}

TEST_F(ThreadPoolTest,HasNoWorkAfterLastWorkIsPulled) {
    pool.add([]{});
    pool.add([]{});
    auto work1 = pool.pull();
    auto work2 = pool.pull();
    ASSERT_THAT(pool.hasWork(), Eq(0));
}

TEST_F(ThreadPoolTest,HasWorkAfterOneWorkIsPulled) {
    pool.add([]{});
    pool.add([]{});
    auto work = pool.pull();
    ASSERT_THAT(pool.hasWork(), Eq(1));
}

TEST_F(ThreadPoolTest, PullsWorkInAThread) {
    pool.start(4);
    bool wasWorked{0};
    std::function<void()> work = [&]() { 
         std::unique_lock<std::mutex> lock(m);
         wasWorked = true;
         wasExecuted.notify_all();
    };

    pool.add(work);
    std::unique_lock<std::mutex> lock(m);
    ASSERT_THAT(wasExecuted.wait_for(lock, std::chrono::milliseconds(100), [&] { return wasWorked; }), Eq(true));      
}

TEST_F(ThreadPoolTest, ExecutesMultipleWork) {
    pool.start(4);
    unsigned int NumberOfWorkItems{3};
    std::function<void()> work = [&]() { incrementCountAndNotify(); };
    
    for(unsigned int i{0}; i < NumberOfWorkItems ; i++) {
        pool.add(work);
    } 

    waitForNotificationOrFailOnTimeout(NumberOfWorkItems);
}

TEST_F(ThreadPoolTest, DispatchMultipleClientThreads) {
    pool.start(4);
    unsigned int NumberOfWorkItems{10};
    unsigned int NumberOfThreads{10};

    std::function<void()> work = [&]() { incrementCountAndNotify(); };
    
    for(unsigned int i{0}; i < NumberOfThreads; i++) {
        threads.push_back(std::make_shared<std::thread>([&] { 
            for(unsigned int j{0}; j < NumberOfWorkItems; j++) 
                pool.add(work); 
        })); 
    }

    waitForNotificationOrFailOnTimeout(NumberOfThreads * NumberOfWorkItems);

}

TEST_F(ThreadPoolTest, MakesSureAllThreadsWorkToRetrieveFromQueue) {
    unsigned int NumberOfThreads=4;
    pool.start(NumberOfThreads);
    std::set<std::thread::id> threadIds;

    std::function<void()> work = [&]() { 
       threadIds.insert(std::this_thread::get_id()); 
       incrementCountAndNotify(); 
    };
    
    unsigned int NumberOfWorkItems{500};
    for(unsigned int j{0}; j < NumberOfWorkItems; j++) { 
        pool.add(work); 
    }
    waitForNotificationOrFailOnTimeout(NumberOfWorkItems);
    ASSERT_THAT(threadIds.size(),Eq(NumberOfThreads));
 
}

TEST_F(ThreadPoolTest, FutureReturningInt) {
    pool.start(4);

    auto work = []() -> int { return 1000; } ;
    auto result = pool.submit(work);
    ASSERT_THAT(result.get(), Eq(1000));
    
}

TEST_F(ThreadPoolTest, FutureReturningString) {
    pool.start(4);

    auto work = []() -> std::string { return "1000"; } ;
    auto result = pool.submit(work);
    ASSERT_THAT(result.get(), StrEq("1000"));
} 
  
TEST_F(ThreadPoolTest,FactorialTest) {
    pool.start(4);
    auto work = [](int n) {
      unsigned long long factorial = 1;
      for(int i = 1; i <=n; ++i) {
        factorial *= i;
      }
      
      return factorial;

    };
    
    auto result = pool.submit(work,12);
    
    ASSERT_THAT(result.get(), Eq(479001600));
}

TEST_F(ThreadPoolTest,TimingTestWithFuture) {
    pool.start(4);
    std::vector<std::future<unsigned long long>> results;
    auto work = [](int n) {
      unsigned long long factorial = 1;
      for(int i = 1; i <=n; ++i) {
        factorial *= i;
      }
      
      return factorial;

    };
    
    
    TestTimer timer("4-sized-TP with Future",0);
    for (int i = 5; i < 60 ; i++) {
        results.push_back(pool.submit(work,i));
    }
    
    
    for(unsigned int i = 0; i< results.size(); i++) {
        results.at(i).get();
    }
}

TEST_F(ThreadPoolTest,TimingTestWithCallback) {
    pool.start(4);
    // core dumps
    std::vector<unsigned long long> results;
    TestTimer timer("4-sized-TP-Callback",0);
    for (int n = 5; n < 60 ; n++) {
        auto work = [&]() {
            unsigned long long factorial = 1;
            for(int i = 1; i <=n; ++i) {
              factorial *= i;
            }
            {
                std::lock_guard<std::mutex> guard(m); 
                results.push_back(factorial);
            }
            incrementCountAndNotify();
        };
        
        pool.add(work);
    }
    
    waitForNotificationOrFailOnTimeout(55);
}

TEST_F(ThreadPoolTest,TimingTestWithoutTP) {
    
    std::vector<unsigned long long> results;
    auto work = [](int n) {
      unsigned long long factorial = 1;
      for(int i = 1; i <=n; ++i) {
        factorial *= i;
      }
      
      return factorial;

    };
    
    
    TestTimer timer("In Sequence",0);
    for (int i = 5; i < 60 ; i++) {
        results.push_back(work(i));
    }
    
     for(unsigned int i = 0; i< results.size(); i++) {
        results.at(i);
    }
    
}

