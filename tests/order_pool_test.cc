#include <gtest/gtest.h>
#include "order_pool.h"
#include <vector>

TEST(OrderPoolTest, AllocateDeallocateReuse) {
    constexpr size_t N = 16;
    OrderPool<N> pool;
    std::vector<int32_t> indices;

    for (size_t i = 0; i < N; ++i) {
        int32_t idx = pool.allocate();
        EXPECT_NE(idx, -1);
        indices.push_back(idx);
        auto& o = pool.get(idx);
        o.order_id = static_cast<uint64_t>(i + 1);
        o.qty = 100;
    }

    EXPECT_EQ(pool.allocate(), -1);

    for (size_t i = 0; i < N/2; ++i) {
        pool.deallocate(indices[i]);
    }

    for (size_t i = 0; i < N/2; ++i) {
        int32_t idx = pool.allocate();
        EXPECT_NE(idx, -1);
        auto& o = pool.get(idx);
        o.qty = 1;
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
