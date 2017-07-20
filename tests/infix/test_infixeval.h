#ifndef TEST_INFIXEVAL_H
#define TEST_INFIXEVAL_H

#include "infixeval.h"
#include "gtest/gtest.h"

using namespace InFix;

namespace TestInFix {

class TestInFix : public ::testing::Test {

protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    // You can do set-up work for each test here.
    TestInFix() {}

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~TestInFix() {}

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp() {}

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown() {}

    // Objects declared here can be used by all tests in the test case for Foo.
    InFix::Evaluator<int> ev;
};

class TestInFixExit : public TestInFix {

protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    // You can do set-up work for each test here.
    TestInFixExit() {}

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~TestInFixExit() {}

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp() {}

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown() {}

    // Objects declared here can be used by all tests in the test case for Foo.
};

}

#endif // TEST_INFIXEVAL_H
