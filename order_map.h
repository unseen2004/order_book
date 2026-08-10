#ifndef ORDER_MAP_H
#define ORDER_MAP_H

#include <array>
#include <cstddef>
#include <cstdint>

template <std::size_t Capacity>
class OrderMap {
    static_assert(Capacity != 0, "Capacity must be non-zero");
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

private:
    std::array<uint64_t, Capacity> keys_{};
    std::array<int32_t, Capacity> values_{};

    static constexpr std::size_t mask() {
        return Capacity - 1;
    }

    void reinsert_cluster(std::size_t empty_slot) {
        std::size_t slot = (empty_slot + 1) & mask();

        while (keys_[slot] != 0) {
            uint64_t key = keys_[slot];
            int32_t value = values_[slot];

            keys_[slot] = 0;
            values_[slot] = -1;

            std::size_t insert_slot = key & mask();
            while (keys_[insert_slot] != 0) {
                insert_slot = (insert_slot + 1) & mask();
            }

            keys_[insert_slot] = key;
            values_[insert_slot] = value;
            slot = (slot + 1) & mask();
        }
    }

public:
    OrderMap() {
        keys_.fill(0);
        values_.fill(-1);
    }

    void insert(uint64_t order_id, int32_t pool_index) {
        if (order_id == 0) {
            return;
        }

        std::size_t slot = static_cast<std::size_t>(order_id) & mask();

        while (keys_[slot] != 0 && keys_[slot] != order_id) {
            slot = (slot + 1) & mask();
        }

        keys_[slot] = order_id;
        values_[slot] = pool_index;
    }

    int32_t get(uint64_t order_id) const {
        if (order_id == 0) {
            return -1;
        }

        std::size_t slot = static_cast<std::size_t>(order_id) & mask();

        while (keys_[slot] != 0 && keys_[slot] != order_id) {
            slot = (slot + 1) & mask();
        }

        if (keys_[slot] == 0) {
            return -1;
        }

        return values_[slot];
    }

    void remove(uint64_t order_id) {
        if (order_id == 0) {
            return;
        }

        std::size_t slot = static_cast<std::size_t>(order_id) & mask();

        while (keys_[slot] != 0 && keys_[slot] != order_id) {
            slot = (slot + 1) & mask();
        }

        if (keys_[slot] == 0) {
            return;
        }

        keys_[slot] = 0;
        values_[slot] = -1;
        reinsert_cluster(slot);
    }
};

#endif
