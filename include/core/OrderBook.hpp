#pragma once

#include "core/LimitLevel.hpp"
#include <map>
#include <unordered_map>

namespace LOB {

    class OrderBook {
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

        // 1. The "Index" (Trees)
        // Bids: Sorted High to Low (std::greater) -> Highest Bid is Best
        std::map<Price, LimitLevel*, std::greater<Price>> bids;

        // Asks: Sorted Low to High (std::less, default) -> Lowest Ask is Best
        std::map<Price, LimitLevel*> asks;

        // 2. The "Lookup" (Hash Map)
        // Maps OrderID -> Order Pointer. 
        // Allows O(1) access to cancel orders.
        std::unordered_map<OrderId, Order*> orderLookup;

        // --- Internal Helpers ---
        
        // Matches an incoming order against the opposite book
        // Returns the remaining quantity of the incoming order
        Quantity match(Order* order);

        // Adds a resting order (that didn't fully match) to the book
        void addRestingOrder(Order* order);
        
        // Helper to check if a trade is possible
        bool canMatch(Side side, Price price) const;
        
        // Helper to get the best price level (Top of Book)
        // returns nullptr if book is empty
        LimitLevel* getBestLevel(Side side) const;
    };
}