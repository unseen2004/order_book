#include <gtest/gtest.h>
#include "order_map.h"

TEST(OrderMapTest, InsertGetAndRemoveWithCollisions) {
    OrderMap<8> map;

    map.insert(1, 10);
    map.insert(9, 20);
    map.insert(17, 30);

    EXPECT_EQ(map.get(1), 10);
    EXPECT_EQ(map.get(9), 20);
    EXPECT_EQ(map.get(17), 30);
    EXPECT_EQ(map.get(25), -1);

    map.remove(9);

    EXPECT_EQ(map.get(1), 10);
    EXPECT_EQ(map.get(9), -1);
    EXPECT_EQ(map.get(17), 30);
}

TEST(OrderMapTest, UpdateExistingKey) {
    OrderMap<8> map;

    map.insert(42, 7);
    EXPECT_EQ(map.get(42), 7);

    map.insert(42, 99);
    EXPECT_EQ(map.get(42), 99);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
