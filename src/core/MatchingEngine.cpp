#include "core/MatchingEngine.hpp"
#include "core/OrderBook.hpp"
#include "core/LimitLevel.hpp"
#include <iostream>
#include <algorithm> // For std::min

namespace LOB {

    Quantity MatchingEngine::match(Order* incomingOrder, OrderBook& book) {
        Quantity qty = incomingOrder->quantity;
        
        while (true) {
            // 1. If fully filled, stop.
            if (qty == 0) break;

            // 2. Identify the side we are matching against
            // If Buying, we look at Asks. If Selling, look at Bids.
            LimitLevel* bestLevel = book.getBestLevel(incomingOrder->side);

            // 3. If book is empty, stop.
            if (bestLevel == nullptr) break;

            // 4. Check if Prices Cross (Spread Crossing)
            if (incomingOrder->side == Side::Buy) {
                // Buyer wants to pay LESS or EQUAL to their limit.
                // If Best Ask > Buy Limit, no trade.
                if (bestLevel->getPrice() > incomingOrder->price) break;
            } else {
                // Seller wants to sell MORE or EQUAL to their limit.
                // If Best Bid < Sell Limit, no trade.
                if (bestLevel->getPrice() < incomingOrder->price) break;
            }

            // 5. Walk the Book (FIFO Execution)
            Order* bookOrder = bestLevel->getHead();

            while (bookOrder != nullptr && qty > 0) {
                Quantity tradeQty = std::min(qty, bookOrder->quantity);

                // --- EXECUTION (Log trade) ---
                // PERFORMANCE NOTE: Comment this out when running benchmarks!
                // std::cout << "Trade Executed: " << tradeQty << " @ " << bestLevel->getPrice() << std::endl;

                // Update Quantities
                qty -= tradeQty;
                bookOrder->fill(tradeQty);

                Order* nextOrder = bookOrder->next;

                // 6. If book order is empty, clean it up
                if (bookOrder->isFilled()) {
                    // Remove from Linked List
                    bestLevel->remove(bookOrder);
                    
                    // Remove from HashMap
                    book.orderLookup.erase(bookOrder->id);
                    
                    // --- CHANGED: Return to Pool instead of Delete ---
                    book.orderPool.deallocate(bookOrder);
                }

                bookOrder = nextOrder;
            }

            // 7. Clean up the Price Level if it's now empty
            if (bestLevel->isEmpty()) {
                if (incomingOrder->side == Side::Buy) {
                    // We ate all the sellers at this price
                    book.asks.erase(bestLevel->getPrice());
                } else {
                    // We ate all the buyers at this price
                    book.bids.erase(bestLevel->getPrice());
                }
                // LimitLevels are not pooled in this version, so standard delete is correct
                delete bestLevel;
            }
        }

        return qty;
    }
}