//
// Created by maks on 8/8/26.
//

#ifndef HFT_LOGEVENT_H
#define HFT_LOGEVENT_H

#include <cstdint>

enum class LogType : uint8_t {
    ORDER_NEW,
    ORDER_FILL,
    ORDER_CANCEL
};

// Must be trivially copyable so it moves through the queue purely as bytes
struct LogEvent {
    uint64_t timestamp; // Grabbed via CPU cycle counter (__rdtsc)
    uint64_t order_id;
    double price;
    uint32_t qty;
    LogType type;
};

#endif //HFT_LOGEVENT_H
