#include "gmock/gmock.h" 

#include "Work.h"

using namespace testing;

TEST(WorkTest,ExecuteAFunction) {
    int a{0};
    auto lambdaf = [&] () { a=1; };
    Work work(lambdaf);
    work.execute();
    ASSERT_THAT(a,1);
}

TEST(WorkTest, CallBackFunction) {
    std::vector<int> result; 
    auto callback =  [&] (int a) { result.push_back(a); };
    auto lambdaf = [&] () { callback(2); };
    Work work(lambdaf);
    work.execute();
    ASSERT_THAT(result, ElementsAre(2));
}
