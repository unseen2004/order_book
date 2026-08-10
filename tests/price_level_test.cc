#include <gtest/gtest.h>
#include "order_pool.h"
#include "price_lelvel.h"

TEST(PriceLevelTest, RemoveMiddleKeepsNeighborsLinked) {
    OrderPool<100> pool;
    PriceLevel<100> level;

    int32_t a = pool.allocate();
    int32_t b = pool.allocate();
    int32_t c = pool.allocate();

    ASSERT_NE(a, -1);
    ASSERT_NE(b, -1);
    ASSERT_NE(c, -1);

    level.append(a, pool);
    level.append(b, pool);
    level.append(c, pool);

    level.remove(b, pool);

    ASSERT_EQ(pool.get(a).next, c);
    ASSERT_EQ(pool.get(c).prev, a);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
