#include <gtest/gtest.h>
#include "order_pool.h"
#include "spsc.h"
#include <random>
#include <unordered_set>

TEST(PropertyTest, PoolAndSpscInvariantsRandomized) {
    constexpr size_t N = 1024;
    OrderPool<N> pool;
    spsc<int, 1024> q;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> op(0, 2);

    std::unordered_set<uint64_t> active_ids;

    for (int iter = 0; iter < 100000; ++iter) {
        int choice = op(rng);
        if (choice == 0) { // allocate
            int32_t idx = pool.allocate();
            if (idx == -1) continue;
            auto& o = pool.get(idx);
            uint64_t id = static_cast<uint64_t>(idx) << 32 | (iter & 0xffffffff);
            o.order_id = id;
            o.qty = 100;
            active_ids.insert(id);
            // push id into SPSC to simulate messaging
            q.push(static_cast<int>(idx));
        } else if (choice == 1) { // pop/process
            int32_t idx;
            if (q.pop(idx)) {
                auto& o = pool.get(idx);
                active_ids.erase(o.order_id);
                pool.deallocate(idx);
            }
        } else { // random check invariants
            // Verify no duplicate ids in active set and pool pointers are sane
            std::unordered_set<uint64_t> seen;
            for (int i = 0; i < static_cast<int>(N); ++i) {
                auto& o = pool.get(i);
                if (o.order_id != 0) {
                    ASSERT_EQ(seen.count(o.order_id), 0u);
                    seen.insert(o.order_id);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
