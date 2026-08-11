#ifndef LIMIT_ORDER_BOOK_H
#define LIMIT_ORDER_BOOK_H

#include <array>
#include <cstdint>
#include <algorithm>
#include "order_map.h"
#include "order_pool.h"
#include "price_level.h"

template <std::size_t Capacity>
class LimitOrderBook {
    static_assert(Capacity != 0, "Capacity must be non-zero");
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");

public:
    static constexpr std::size_t kMaxPriceLevels = 100000;

private:
    OrderPool<Capacity> pool_;
    OrderMap<Capacity> order_map_;
    std::array<PriceLevel<Capacity>, kMaxPriceLevels> bids_{};
    std::array<PriceLevel<Capacity>, kMaxPriceLevels> asks_{};
    uint32_t best_bid_ = 0;
    uint32_t best_ask_ = static_cast<uint32_t>(kMaxPriceLevels);

    static constexpr bool valid_price(uint32_t price) {
        return price < kMaxPriceLevels;
    }

    void refresh_best_bid() {
        while (best_bid_ > 0 && bids_[best_bid_].head == -1) {
            --best_bid_;
        }
    }

    void refresh_best_ask() {
        while (best_ask_ < kMaxPriceLevels && asks_[best_ask_].head == -1) {
            ++best_ask_;
        }
        if (best_ask_ >= kMaxPriceLevels) {
            best_ask_ = static_cast<uint32_t>(kMaxPriceLevels);
        }
    }

public:
    void add_order(uint64_t order_id, uint32_t price, uint32_t qty, bool is_buy) {
        if (order_id == 0 || !valid_price(price)) {
            return;
        }

        int32_t pool_index = pool_.allocate();
        if (pool_index == -1) {
            return;
        }

        Order& order = pool_.get(pool_index);
        order.order_id = order_id;
        order.price = price;
        order.qty = qty;
        order.is_buy = is_buy;

        order_map_.insert(order_id, pool_index);

        if (is_buy) {
            bids_[price].append(pool_index, pool_);
            if (price > best_bid_) {
                best_bid_ = price;
            }
        } else {
            asks_[price].append(pool_index, pool_);
            if (price < best_ask_) {
                best_ask_ = price;
            }
        }
    }

    void cancel_order(uint64_t order_id) {
        int32_t pool_index = order_map_.get(order_id);
        if (pool_index == -1) {
            return;
        }

        Order& order = pool_.get(pool_index);
        uint32_t price = order.price;
        bool is_buy = order.is_buy;

        if (is_buy) {
            bids_[price].remove(pool_index, pool_);
        } else {
            asks_[price].remove(pool_index, pool_);
        }

        order_map_.remove(order_id);
        pool_.deallocate(pool_index);

        if (is_buy && price == best_bid_ && bids_[price].head == -1) {
            refresh_best_bid();
        }

        if (!is_buy && price == best_ask_ && asks_[price].head == -1) {
            refresh_best_ask();
        }
    }

    uint32_t best_bid() const {
        return best_bid_;
    }

    uint32_t best_ask() const {
        return best_ask_;
    }

    int32_t order_index(uint64_t order_id) const {
        return order_map_.get(order_id);
    }

    Order& order_at(int32_t pool_index) {
        return pool_.get(pool_index);
    }

    void process_order(uint64_t order_id, uint32_t price, uint32_t qty, bool is_buy) {
        if (order_id == 0 || !valid_price(price) || qty == 0) {
            return;
        }

        while (qty > 0) {
            if (is_buy) {
                // Crossing check: there are asks AND incoming price >= best ask
                if (!(best_ask_ < kMaxPriceLevels && price >= best_ask_)) {
                    break;
                }

                uint32_t target_price = best_ask_;
                int32_t head_idx = asks_[target_price].head;
                if (head_idx == -1) {
                    refresh_best_ask();
                    continue;
                }

                Order& resting = pool_.get(head_idx);
                uint32_t trade_qty = std::min(qty, resting.qty);

                qty -= trade_qty;
                resting.qty -= trade_qty;

                // Optional: emit trade event here
                // std::cout << "TRADE: " << trade_qty << " @ " << resting.price << "\n";

                if (resting.qty == 0) {
                    cancel_order(resting.order_id);
                }
            } else {
                // Crossing check: there are bids AND incoming price <= best bid
                if (!(best_bid_ > 0 && price <= best_bid_)) {
                    break;
                }

                uint32_t target_price = best_bid_;
                int32_t head_idx = bids_[target_price].head;
                if (head_idx == -1) {
                    refresh_best_bid();
                    continue;
                }

                Order& resting = pool_.get(head_idx);
                uint32_t trade_qty = std::min(qty, resting.qty);

                qty -= trade_qty;
                resting.qty -= trade_qty;

                // Optional: emit trade event here
                // std::cout << "TRADE: " << trade_qty << " @ " << resting.price << "\n";

                if (resting.qty == 0) {
                    cancel_order(resting.order_id);
                }
            }
        }

        if (qty > 0) {
            add_order(order_id, price, qty, is_buy);
        }
    }
};

#endif
