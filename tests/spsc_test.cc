#include <gtest/gtest.h>
#include "spsc.h"

TEST(SpscTest, PushPopOrder) {
    spsc<int, 8> q;

    // Fill up to capacity (capacity is N-1 because one slot is used to distinguish full/empty)
    for (int i = 0; i < 7; ++i) {
        EXPECT_TRUE(q.push(i+1));
    }
    // Now queue should be full
    EXPECT_FALSE(q.push(100));

    int v;
    for (int i = 0; i < 7; ++i) {
        EXPECT_TRUE(q.pop(v));
        EXPECT_EQ(v, i+1);
    }
    EXPECT_FALSE(q.pop(v));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
