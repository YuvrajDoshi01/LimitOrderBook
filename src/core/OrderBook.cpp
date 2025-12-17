#include "core/OrderBook.hpp"
#include "core/MatchingEngine.hpp"
#include "types/Order.hpp"
#include <iostream>
#define endl std::endl

namespace LOB {

    OrderBook::OrderBook(): orderPool(150000000), levelPool(1024) {
        // Pre-allocate lookup vector for 120M orders with modest initial size.
        orderLookup.reserve(120000000);
    }

    OrderBook::~OrderBook() {
        // Destructor
    }

    void OrderBook::addOrder(OrderId id, Side side, Price price, Quantity quantity) {
        // Step 1: Duplicate Check (Vector Lookup)
        if (id < orderLookup.size() && orderLookup[id] != nullptr) {
            return; 
        }

        // Step 2: Allocation from Pool
        Order* order = orderPool.allocate(id, side, price, quantity);

        // Step 3: Match Immediately (Market Taker)
        Quantity remainingQty = MatchingEngine::match(order, *this);
        
        // Step 4: Update the Order
        order->quantity = remainingQty;

        // Step 5: Decide Fate
        if (remainingQty > 0){
            addRestingOrder(order);
        }
        else{
            orderPool.deallocate(order);
        }
    }

    void OrderBook::addRestingOrder(Order* order) {
        // 1. Grow vector if needed (smart exponential growth to avoid thrashing)
        if (order->id >= orderLookup.size()) {
            size_t newSize = std::max((size_t)(order->id + 1), orderLookup.size() * 2);
            orderLookup.resize(newSize, nullptr);
        }

        // 2. Direct Assignment
        orderLookup[order->id] = order;

        // 3. Flat structure logic
        LimitLevel* level = getOrCreateLevel(order->side, order->price);
        level->append(order);
    }

    void OrderBook::cancelOrder(OrderId id) { 
        // 1. Bounds Check
        if (id >= orderLookup.size()) {
            return; 
        }

        // 2. Null Check
        Order* order = orderLookup[id];
        if (order == nullptr) {
            return; 
        }

        LimitLevel* parentLimit = order->parentLimit;

        // 3. Remove from List
        parentLimit->remove(order);

        // 4. Tree Cleanup
        if(parentLimit->isEmpty()){
            removeLevel(order->side, parentLimit->getPrice());
        }

        // 5. Vector Cleanup (Crucial Change)
        // OLD: orderLookup.erase(id);  <-- This was the bug in your snippet
        // NEW: Just nullify the slot
        orderLookup[id] = nullptr;

        // 6. Return to Pool
        orderPool.deallocate(order);
    }

    Quantity OrderBook::getVolumeAtPrice(Side side, Price price) const {
        LimitLevel* lvl = findLevel(side, price);
        return (lvl == nullptr) ? 0 : lvl->getVolume();
    }

    bool OrderBook::hasOrder(OrderId id) const {
        return id < orderLookup.size() && orderLookup[id] != nullptr;
    }

    LimitLevel* OrderBook::getBestLevel(Side side) const {
        return (side == Side::Buy) ? bestAsk : bestBid;
    }

    LimitLevel* OrderBook::findLevel(Side side, Price price) const {
        const auto& book = (side == Side::Buy) ? bids : asks;
        auto comp = (side == Side::Buy)
            ? [](const LevelEntry& e, Price p){ return e.first > p; }
            : [](const LevelEntry& e, Price p){ return e.first < p; };
        auto it = std::lower_bound(book.begin(), book.end(), price, comp);
        if (it != book.end() && it->first == price) {
            return it->second;
        }
        return nullptr;
    }

    LimitLevel* OrderBook::getOrCreateLevel(Side side, Price price) {
        auto& book = (side == Side::Buy) ? bids : asks;
        auto comp = (side == Side::Buy)
            ? [](const LevelEntry& e, Price p){ return e.first > p; }
            : [](const LevelEntry& e, Price p){ return e.first < p; };
        auto it = std::lower_bound(book.begin(), book.end(), price, comp);
        if (it != book.end() && it->first == price) {
            return it->second;
        }
        LimitLevel* level = levelPool.allocate(price);
        it = book.insert(it, LevelEntry{price, level});
        (void)it; // suppress unused warning
        refreshBestPointers();
        return level;
    }

    void OrderBook::removeLevel(Side side, Price price) {
        auto& book = (side == Side::Buy) ? bids : asks;
        auto comp = (side == Side::Buy)
            ? [](const LevelEntry& e, Price p){ return e.first > p; }
            : [](const LevelEntry& e, Price p){ return e.first < p; };
        auto it = std::lower_bound(book.begin(), book.end(), price, comp);
        if (it != book.end() && it->first == price) {
            levelPool.deallocate(it->second);
            book.erase(it);
            refreshBestPointers();
        }
    }

    void OrderBook::refreshBestPointers() {
        bestBid = bids.empty() ? nullptr : bids.front().second;
        bestAsk = asks.empty() ? nullptr : asks.front().second;
    }
}