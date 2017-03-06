
# Motivation
In birthday_paradox , i used a classic threadpool. Here I try to write a threadpool that returns  a future ala Java.
I am presenting a short demo on Java Future and Executor at work, so this would be a nice way of showing a C++ ExecutorService implementation

# Code design 
The change is at adding tasks, in the classic threadpool , it uses callback to update the result of the async process, here it would be returning a future. The issue seems to be that std::packaged_task is not copyable but moveable only but std::function is expecting something that is copyable.

to do that , I had to understand the following C++ 11 syntax/concepts

- type deduction for universal references
- std::forward
- std::packaged_task
- std::bind
- moveable and copyable

In http://www.mathematik.uni-ulm.de/numerik/hpc/ws15/uebungen/session19/hpc/mt/thread_pool.h.html and https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h the authors have used shared_ptr to wrap the std::packaged_task as shared_ptrs are copyable and moveable.

Anthony Williams in his book Concurrent C++ in Action uses a function wrapper to replace std::function so that it can be moveable.

# Future plans
Improve the feature set
- Work stealing
- Priority queue
- Lock free data structures ?
