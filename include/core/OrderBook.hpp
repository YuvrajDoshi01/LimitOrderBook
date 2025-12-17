#pragma once

#include "core/LimitLevel.hpp"
#include "memory/ObjectPool.hpp"
#include "memory/LimitLevelPool.hpp"
#include <vector>
#include <utility>
#include <algorithm>

namespace LOB {

    class OrderBook {
    friend class MatchingEngine;
    public:
        // Constructor & Destructor
        OrderBook();
        ~OrderBook();

        // --- The Main API ---
        
        // Adds a new order to the book
        // Returns the number of trades executed immediately
        void addOrder(OrderId id, Side side, Price price, Quantity quantity);

        // Cancels an existing order
        void cancelOrder(OrderId id);

        // --- Getters (For testing/verification) ---
        Quantity getVolumeAtPrice(Side side, Price price) const;
        bool hasOrder(OrderId id) const;

    private:
        // --- Data Structures ---
        OrderPool orderPool;
        LimitLevelPool levelPool;

        using LevelEntry = std::pair<Price, LimitLevel*>;
        // Bids: sorted descending by price
        std::vector<LevelEntry> bids;
        // Asks: sorted ascending by price
        std::vector<LevelEntry> asks;

        // Cached best levels to avoid repeated begin() calls
        LimitLevel* bestBid {nullptr};
        LimitLevel* bestAsk {nullptr};

        // 2. The "Lookup" (Hash Map)
        // Maps OrderID -> Order Pointer. 
        // Allows O(1) access to cancel orders.
        std::vector<Order*> orderLookup;

        // --- Internal Helpers ---
        
        // Adds a resting order (that didn't fully match) to the book
        void addRestingOrder(Order* order);

        // Helper to get the best price level (Top of Book)
        // returns nullptr if book is empty
        LimitLevel* getBestLevel(Side side) const;

        // Find a level if it exists (nullptr if absent)
        LimitLevel* findLevel(Side side, Price price) const;

        // Get or create a level, inserting in sorted position
        LimitLevel* getOrCreateLevel(Side side, Price price);

        // Remove an empty level and update cached best
        void removeLevel(Side side, Price price);

        // Update cached best pointers after insert/remove
        void refreshBestPointers();
    };
}