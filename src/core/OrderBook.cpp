#include "core/OrderBook.hpp"
#include "core/MatchingEngine.hpp"
#include "types/Order.hpp"
#include <iostream> 
#define endl std::endl

namespace LOB {

    OrderBook::OrderBook(): orderPool(10000000) {
        // Resize vector to hold 1 Million IDs initially.
        // We fill it with 'nullptr' to indicate empty slots.
        orderLookup.resize(10000000, nullptr);
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

    // NOTE: You can DELETE this match() function from OrderBook.cpp entirely.
    // You are now using MatchingEngine::match(), so this code is dead/duplicate.
    // If you keep it, the compiler might complain or you might call the wrong one.
    
    /* Quantity OrderBook::match(Order* incomingOrder) { 
        // DELETE THIS WHOLE FUNCTION 
    }
    */

    void OrderBook::addRestingOrder(Order* order) {
        // 1. Dynamic Resizing (Safety Check)
        if (order->id >= orderLookup.size()) {
            orderLookup.resize(order->id * 2, nullptr);
        }

        // 2. Direct Assignment
        orderLookup[order->id] = order;

        // 3. Tree Logic
        LimitLevel* level = nullptr;
        if(order->side == Side::Buy){
            auto it = bids.find(order->price);
            if(it != bids.end()){
                level = it->second;
            }
            else{
                level = new LimitLevel(order->price);
                bids[order->price] = level;
            }
        }
        else{
            auto it = asks.find(order->price);
            if(it != asks.end()){
                level = it->second;
            }
            else{
                level = new LimitLevel(order->price);
                asks[order->price] = level;
            }
        }
        
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
            if(order->side == Side::Buy){
                bids.erase(parentLimit->getPrice());
            }
            else{
                asks.erase(parentLimit->getPrice());
            }
            delete parentLimit;
        }

        // 5. Vector Cleanup (Crucial Change)
        // OLD: orderLookup.erase(id);  <-- This was the bug in your snippet
        // NEW: Just nullify the slot
        orderLookup[id] = nullptr;

        // 6. Return to Pool
        orderPool.deallocate(order);
    }

    LimitLevel* OrderBook::getBestLevel(Side side) const {
        if(side == Side::Buy){
            if(!asks.empty()) return asks.begin()->second;
            return nullptr;
        }
        if(side == Side::Sell){
            if(!bids.empty()) return bids.begin()->second;
            return nullptr;
        }
        return nullptr;
    }
}