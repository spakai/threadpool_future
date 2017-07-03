
# Motivation
In birthday_paradox , i used a classic threadpool. Here I try to write a threadpool that returns  a future ala Java.
I am presenting a short demo on Java Future and Executor at work, so this would be a nice way of showing a C++ ExecutorService implementation

# Code design 

We need

i) A way to create threads and hold them in an idle state. 

ii) A container to hold the tasks and which a client can push the tasks into.

iii)  Have threads in the pool remove waiting tasks from the container and execute them. 

I've used TDD to help write the code. TDD allows us to start with a simple design and move on from there, gaining confidence along the way. 

Many times we encounter a situation where we want a thread to return a result instead of updating the result indirectly. Futures allow assignment to a value that is to be calculated at a later time.

What this means is  when a client passes a work to the threadpool, it gets a future object which it can retrieve the result of the work later.
The keyword auto is used as the return type since it can be `std::future<int>` or `std::future<std::string>` etc. 
F is basically either `std::function<int>` or `std::function<std::string>` etc . The return type of F is  deduced using decltype which will also be the return type of the std::future.The task is then packaged as std:::packaged_task so that it can be accessed via a std::future object using get_future(). The packaged task is put behind a shared pointer because packaged_tasks are just move-constructible but std::function requires the callable object to be copy-constructible. shared_ptrs are copy-constructible. We can now push the task into the queue. There is a lot of work done here just to repackage packaged_task into std::function, we can do better, maybe having std::queue to take in packaged_tasks instead of std::function.


# Future plans
Improve the feature set
- Work stealing
- Concurrent queue
- Lock free data structures
- Take in features of ThreadPoolExecutor such as core pool size, max pool size, keep alive time, construct threads on demand
