#pragma once

#include <functional>

class Work{
    private:
        int id;
        std::function<void()> executeFunction;
    public:
        Work(int id=0)
            : id(id)
            , executeFunction([]{})
        {}
        Work(std::function<void()> executeFunction, int id=0)
            : id(id)
            , executeFunction(executeFunction)
        {} 

        int getId() const;
        void execute(); 
};