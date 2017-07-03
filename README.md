
# Motivation
In birthday_paradox , i used a classic threadpool. Here I try to write a threadpool that returns  a future ala Java.
I am presenting a short demo on Java Future and Executor at work, so this would be a nice way of showing a C++ ExecutorService implementation

# Code design 

We need

i) A way to create threads and hold them in an idle state. 

ii) A container to hold the tasks and which a client can push the tasks into.

iii)  Have threads in the pool remove waiting tasks from the container and execute them. 

I've used TDD to help write the code. 


# Future plans
Improve the feature set
- Work stealing
- Concurrent queue
- Lock free data structures
- Take in features of ThreadPoolExecutor such as core pool size, max pool size, keep alive time, construct threads on demand
