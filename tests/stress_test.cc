#include <gtest/gtest.h>
#include "order_pool.h"
#include <chrono>

TEST(StressTest, AllocationThroughput) {
    constexpr size_t N = 1<<20; // 1M
    OrderPool<1024> pool; // small pool to test exhaustion behavior

    auto start = std::chrono::high_resolution_clock::now();
    int64_t ops = 0;
    for (int i = 0; i < 1000000; ++i) {
        int32_t idx = pool.allocate();
        if (idx != -1) {
            pool.deallocate(idx);
            ++ops;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Performed " << ops << " alloc/dealloc ops in " << dur << " ms\n";
    ASSERT_GT(ops, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
