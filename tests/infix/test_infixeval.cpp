#include "test_infixeval.h"

#define CheckResultOf(s,r) do {                                         \
    ev.clear();                                                         \
    ev.set(std::string( #s ));                                          \
    EXPECT_EQ(ev.getValue(), r);                                        \
    } while (0)

namespace TestInFix {

TEST_F(TestInFix, SimpleInfixOperations1) {
    
    CheckResultOf(10 + 2 * 6, 22);

}

TEST_F(TestInFix, SimpleInfixOperations2) {
    
    CheckResultOf(100 * 2 + 12, 212);

}

TEST_F(TestInFix, SimpleInfixOperations3) {
    
    CheckResultOf(100 * ( 2 + 12 ), 1400);

}

TEST_F(TestInFix, SimpleInfixOperations4) {
    
    CheckResultOf(100 * ( 2 + 12 ) / 14, 100);    

}

}
