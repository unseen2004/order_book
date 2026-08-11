#ifndef FIX_ENCODER_H
#define FIX_ENCODER_H

#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>

inline char* fast_itoa(char* buffer, uint32_t value) {
    auto result = std::to_chars(buffer, buffer + 10, value);
    return result.ptr;
}

class FixEncoder {
private:
    std::array<char, 256> buffer_{};

    static constexpr char kFixHeader[] = "8=FIX.4.2\x01";
    static constexpr char kMsgTypeNewOrder[] = "35=D\x01";
    static constexpr char kSideBuy[] = "54=1\x01";
    static constexpr char kSideSell[] = "54=2\x01";
    static constexpr char kClOrdIdTag[] = "11=";
    static constexpr char kOrderQtyTag[] = "38=";
    static constexpr char kPriceTag[] = "44=";
    static constexpr char kSoh = '\x01';

    template <std::size_t N>
    static inline void append_literal(char*& out, const char (&literal)[N]) {
        std::memcpy(out, literal, N - 1);
        out += (N - 1);
    }

public:
    std::size_t encode_new_order(uint64_t order_id, uint32_t price, uint32_t qty, bool is_buy) {
        char* out = buffer_.data();

        append_literal(out, kFixHeader);
        append_literal(out, kMsgTypeNewOrder);

        append_literal(out, kClOrdIdTag);
        auto order_result = std::to_chars(out, buffer_.data() + buffer_.size(), order_id);
        out = order_result.ptr;
        *out++ = kSoh;

        if (is_buy) {
            append_literal(out, kSideBuy);
        } else {
            append_literal(out, kSideSell);
        }

        append_literal(out, kOrderQtyTag);
        out = fast_itoa(out, qty);
        *out++ = kSoh;

        append_literal(out, kPriceTag);
        out = fast_itoa(out, price);
        *out++ = kSoh;

        return static_cast<std::size_t>(out - buffer_.data());
    }

    const char* data() const {
        return buffer_.data();
    }
};

#endif // FIX_ENCODER_H
