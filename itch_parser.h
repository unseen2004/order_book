#ifndef ITCH_PARSER_H
#define ITCH_PARSER_H

#include <cstdint>
#include <cstring>
#include "limit_order_book.h"

// Packed struct for ITCH Add Order (no padding)
#pragma pack(push, 1)
struct ItchAddOrder {
    char     message_type;      // 'A'
    uint16_t stock_locate;
    uint16_t tracking_number;
    char     timestamp[6];      // 6-byte integer (skip parsing)
    uint64_t order_reference_number;
    char     buy_sell_indicator; // 'B' or 'S'
    uint32_t shares;
    char     stock[8];          // Ticker symbol, e.g., "AAPL    "
    uint32_t price;
};
#pragma pack(pop)

// Byte-swap helpers: network (big-endian) -> host (little-endian on x86)
inline uint16_t bswap16(uint16_t v) { return __builtin_bswap16(v); }
inline uint32_t bswap32(uint32_t v) { return __builtin_bswap32(v); }
inline uint64_t bswap64(uint64_t v) { return __builtin_bswap64(v); }

// Additional ITCH message types
#pragma pack(push, 1)
struct ItchOrderDelete {
    char message_type; // 'D'
    uint16_t stock_locate;
    uint16_t tracking_number;
    char timestamp[6];
    uint64_t order_reference_number;
};

struct ItchOrderExecuted {
    char message_type; // 'E'
    uint16_t stock_locate;
    uint16_t tracking_number;
    char timestamp[6];
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
};
#pragma pack(pop)

// Parse a single ITCH message buffer and dispatch to the LOB.
// Assumes buffer points to a complete message and uses zero-copy reinterpret_cast.
template <std::size_t Cap>
inline void parse_itch_message(const char* buffer, LimitOrderBook<Cap>& lob) {
    if (buffer == nullptr) return;

    char type = buffer[0];
    if (type == 'A') {
        const ItchAddOrder* msg = reinterpret_cast<const ItchAddOrder*>(buffer);
        uint64_t order_ref = bswap64(msg->order_reference_number);
        uint32_t shares = bswap32(msg->shares);
        uint32_t price = bswap32(msg->price);
        bool is_buy = (msg->buy_sell_indicator == 'B');
        lob.add_order(order_ref, price, shares, is_buy);
        return;
    }

    if (type == 'D') {
        const ItchOrderDelete* msg = reinterpret_cast<const ItchOrderDelete*>(buffer);
        uint64_t order_ref = bswap64(msg->order_reference_number);
        lob.cancel_order(order_ref);
        return;
    }

    if (type == 'E') {
        const ItchOrderExecuted* msg = reinterpret_cast<const ItchOrderExecuted*>(buffer);
        uint64_t order_ref = bswap64(msg->order_reference_number);
        uint32_t exec_shares = bswap32(msg->executed_shares);

        int32_t idx = lob.order_index(order_ref);
        if (idx != -1) {
            Order& resting = lob.order_at(idx);
            if (resting.qty > exec_shares) {
                resting.qty -= exec_shares;
            } else {
                // Fully consumed
                lob.cancel_order(order_ref);
            }
        }
        return;
    }

    // Unhandled types are ignored for now
}

// SoupBinTCP-like framer: reads 2-byte big-endian length header followed by that many bytes
template <std::size_t Cap>
inline void process_soupbin_messages(const char* data, size_t len, LimitOrderBook<Cap>& lob) {
    size_t offset = 0;
    while (offset + sizeof(uint16_t) <= len) {
        uint16_t be_len;
        std::memcpy(&be_len, data + offset, sizeof(be_len));
        uint16_t msg_len = bswap16(be_len);
        offset += sizeof(uint16_t);
        if (msg_len == 0) break;
        if (offset + msg_len > len) break; // incomplete
        parse_itch_message<Cap>(data + offset, lob);
        offset += msg_len;
    }
}

#endif // ITCH_PARSER_H
