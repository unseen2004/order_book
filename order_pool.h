#ifndef ORDER_POOL_H
#define ORDER_POOL_H

#include <cstdint>
#include <array>

// Represent prices as integers (e.g., $150.25 -> 15025) to avoid floating-point math
struct Order {
    uint64_t order_id;
    uint32_t price;
    uint32_t qty;
    bool is_buy;

    // Intrusive linked-list indices. -1 means null/end of list.
    int32_t prev = -1;
    int32_t next = -1;
};

template <std::size_t MaxOrders>
class OrderPool {
private:
    std::array<Order, MaxOrders> pool_;

    // A stack of available indices
    std::array<int32_t, MaxOrders> free_indices_;
    int32_t free_head_;

public:
    OrderPool() {
        // At startup (cold path), every index is pushed onto the free list
        for (int32_t i = 0; i < static_cast<int32_t>(MaxOrders); ++i) {
            free_indices_[i] = i;
        }
        free_head_ = static_cast<int32_t>(MaxOrders) - 1;
    }

    // HOT PATH: O(1) allocation (1 CPU cycle)
    inline int32_t allocate() {
        if (free_head_ < 0) {
            return -1; // Pool exhausted
        }
        return free_indices_[free_head_--];
    }

    // HOT PATH: O(1) deallocation (1 CPU cycle)
    inline void deallocate(int32_t index) {
        // Reset the intrusive pointers before recycling to prevent pointer pollution
        pool_[index].prev = -1;
        pool_[index].next = -1;

        free_indices_[++free_head_] = index;
    }

    // O(1) memory access
    inline Order& get(int32_t index) {
        return pool_[index];
    }
};

#endif // ORDER_POOL_H