#ifndef PRICE_LEVEL_H
#define PRICE_LEVEL_H

#include <cstdint>
#include "order_pool.h"

template <std::size_t MaxOrders>
struct PriceLevel {
    int32_t head = -1;
    int32_t tail = -1;

    // O(1) Append to the end of the queue (Time Priority)
    inline void append(int32_t order_idx, OrderPool<MaxOrders>& pool) {
        Order& order = pool.get(order_idx);

        // This order is going to the back, so nothing is behind it
        order.next = -1;

        if (head == -1) {
            // The list is entirely empty
            order.prev = -1;
            head = order_idx;
            tail = order_idx;
        } else {
            // The list has at least one order. Attach to the current tail.
            order.prev = tail;                     // Point new order back to old tail
            pool.get(tail).next = order_idx;       // Point old tail forward to new order
            tail = order_idx;                      // Update official tail
        }
    }

    // O(1) Splice an order out of the queue
    inline void remove(int32_t order_idx, OrderPool<MaxOrders>& pool) {
        Order& order = pool.get(order_idx);

        // 1. Fix the forward connection (look at the order in front of us)
        if (order.prev != -1) {
            // The order in front of us points its 'next' to the order behind us
            pool.get(order.prev).next = order.next;
        } else {
            // We were the head. The order behind us becomes the new head.
            head = order.next;
        }

        // 2. Fix the backward connection (look at the order behind us)
        if (order.next != -1) {
            // The order behind us points its 'prev' to the order in front of us
            pool.get(order.next).prev = order.prev;
        } else {
            // We were the tail. The order in front of us becomes the new tail.
            tail = order.prev;
        }

        // Note: We do not need to reset order.prev and order.next to -1 here,
        // because OrderPool::deallocate(order_idx) handles that cleanup.
    }
};

#endif // PRICE_LEVEL_H