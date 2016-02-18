
# Motivation
In birthday_paradox , i used a classic threadpool. Here I try to write a threadpool that returns  a future ala Java.
I am presenting a short demo on Java Future and Executor at work, so this would be a nice way of showing a C++ ExecutorService implementation

# Code design 
The change is at adding tasks, in the classic threadpool , it uses callback to update the result of the async process, here it would be returning a future.

to do that , I had to understand the following C++ 11 syntax/concepts

- type deduction for universal references
- std::forward
- std::packaged_task
- std::bind
- moveable not copyable syntax
