#include <gtest/gtest.h>
#include <vector>
#include <cstring>
#include "itch_parser.h"

TEST(ItchParserTest, AddExecuteDeleteFlow) {
    LimitOrderBook<1024> lob;

    std::vector<char> buf;

    // Helper to append a message with 2-byte BE length header
    auto append_msg = [&](const void* msg, size_t msg_size) {
        uint16_t be_len = bswap16(static_cast<uint16_t>(msg_size));
        size_t old = buf.size();
        buf.resize(old + sizeof(be_len) + msg_size);
        std::memcpy(buf.data() + old, &be_len, sizeof(be_len));
        std::memcpy(buf.data() + old + sizeof(be_len), msg, msg_size);
    };

    // Build Add Order (A)
    ItchAddOrder add{};
    add.message_type = 'A';
    add.stock_locate = bswap16(0);
    add.tracking_number = bswap16(0);
    // timestamp left zero
    add.order_reference_number = bswap64(1);
    add.buy_sell_indicator = 'B';
    add.shares = bswap32(100);
    // stock left zero
    add.price = bswap32(500);

    append_msg(&add, sizeof(add));

    // Build Execute Order (E) - execute 40 shares
    ItchOrderExecuted exec{};
    exec.message_type = 'E';
    exec.stock_locate = bswap16(0);
    exec.tracking_number = bswap16(0);
    exec.order_reference_number = bswap64(1);
    exec.executed_shares = bswap32(40);
    exec.match_number = bswap64(123);

    append_msg(&exec, sizeof(exec));

    // Process A + E
    process_soupbin_messages(buf.data(), buf.size(), lob);

    int32_t idx = lob.order_index(1);
    ASSERT_NE(idx, -1);
    EXPECT_EQ(lob.order_at(idx).qty, 60u);

    // Now create Delete (D) buffer and process it
    std::vector<char> delbuf;
    auto append_msg_to = [&](std::vector<char>& target, const void* msg, size_t msg_size) {
        uint16_t be_len = bswap16(static_cast<uint16_t>(msg_size));
        size_t old = target.size();
        target.resize(old + sizeof(be_len) + msg_size);
        std::memcpy(target.data() + old, &be_len, sizeof(be_len));
        std::memcpy(target.data() + old + sizeof(be_len), msg, msg_size);
    };

    ItchOrderDelete del{};
    del.message_type = 'D';
    del.stock_locate = bswap16(0);
    del.tracking_number = bswap16(0);
    del.order_reference_number = bswap64(1);
    append_msg_to(delbuf, &del, sizeof(del));

    process_soupbin_messages(delbuf.data(), delbuf.size(), lob);

    EXPECT_EQ(lob.order_index(1), -1);
}
