#ifndef SPSC_H
#define SPSC_H

#include <atomic>
#include <cstddef>
#include <new>

template <typename T, std::size_t N>
class spsc {
private:
    static_assert((N != 0) && ((N & (N - 1)) == 0), "N must be a power of 2");

    T buffer[N];

    alignas(std::hardware_destructive_interference_size)
    std::atomic<std::size_t> head;

    alignas(std::hardware_destructive_interference_size)
    std::atomic<std::size_t> tail;

public:
    spsc() : head(0), tail(0) {}
    ~spsc() = default;

    bool push(const T& item) {
        std::size_t current_head = head.load(std::memory_order_relaxed);
        std::size_t next_head = (current_head + 1) & (N - 1);

        if (next_head == tail.load(std::memory_order_acquire)) {
            return false;
        }

        buffer[current_head] = item;
        head.store(next_head, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        std::size_t current_tail = tail.load(std::memory_order_relaxed);

        if (current_tail == head.load(std::memory_order_acquire)) {
            return false;
        }

        item = buffer[current_tail];
        tail.store((current_tail + 1) & (N - 1), std::memory_order_release);
        return true;
    }
};

#endif // SPSC_H