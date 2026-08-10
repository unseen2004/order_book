#include <gtest/gtest.h>
#include "limit_order_book.h"

TEST(LimitOrderBookTest, AddUpdatesBestBidAndAsk) {
    LimitOrderBook<16> book;

    book.add_order(1, 10, 100, true);
    book.add_order(2, 20, 100, true);
    book.add_order(3, 15, 100, true);
    book.add_order(4, 30, 100, false);
    book.add_order(5, 25, 100, false);
    book.add_order(6, 40, 100, false);

    EXPECT_EQ(book.best_bid(), 20u);
    EXPECT_EQ(book.best_ask(), 25u);

    int32_t bid_idx = book.order_index(2);
    int32_t ask_idx = book.order_index(5);
    ASSERT_NE(bid_idx, -1);
    ASSERT_NE(ask_idx, -1);
    EXPECT_EQ(book.order_at(bid_idx).order_id, 2u);
    EXPECT_EQ(book.order_at(ask_idx).order_id, 5u);
}

TEST(LimitOrderBookTest, CancelStepsBestPricesBack) {
    LimitOrderBook<16> book;

    book.add_order(1, 10, 100, true);
    book.add_order(2, 20, 100, true);
    book.add_order(3, 15, 100, true);
    book.add_order(4, 30, 100, false);
    book.add_order(5, 25, 100, false);
    book.add_order(6, 40, 100, false);

    book.cancel_order(2);
    EXPECT_EQ(book.best_bid(), 15u);
    EXPECT_EQ(book.order_index(2), -1);

    book.cancel_order(5);
    EXPECT_EQ(book.best_ask(), 30u);
    EXPECT_EQ(book.order_index(5), -1);
}

TEST(LimitOrderBookTest, RemoveOnlyOrderResetsTopOfBook) {
    LimitOrderBook<16> book;

    book.add_order(1, 12, 100, true);
    book.add_order(2, 18, 100, false);

    book.cancel_order(1);
    book.cancel_order(2);

    EXPECT_EQ(book.best_bid(), 0u);
    EXPECT_EQ(book.best_ask(), LimitOrderBook<16>::kMaxPriceLevels);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
