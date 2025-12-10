#include "core/OrderBook.hpp"
#include "core/MatchingEngine.hpp"
#include "types/Order.hpp"
#include <iostream> // For printing trades
#define endl std::endl
namespace LOB {

    OrderBook::OrderBook() {
        // Constructor: Nothing complex to initialize here since 
        // std::map and std::unordered_map initialize themselves.
    }

    OrderBook::~OrderBook() {
        // Destructor:
        // In a real production system, you would iterate through 
        // bids and asks maps and 'delete' all allocated LimitLevel pointers
        // to prevent memory leaks.
        // For now, you can leave this empty or add a simple cleanup loop.
    }

    void OrderBook::addOrder(OrderId id, Side side, Price price, Quantity quantity) {
        // Step 1: Duplicate Check
        // Check if 'id' already exists in 'orderLookup'. 
        // If yes, return immediately (idempotency).
        if(orderLookup.find(id) != orderLookup.end()) {
            return;
        }

        // Step 2: Create the Order Object
        // Allocate a new Order object (using 'new' for now).
        // (Note: In the advanced version, we will ask the ObjectPool for this).
        Order* order = new Order(id, side, price, quantity);
        // Step 3: Match Immediately (Market Taker)
        // Call the 'match()' function passing this new order.
        // It returns the remaining quantity that wasn't filled.
        // Quantity remainingQty = match(order);
        Quantity remainingQty = MatchingEngine::match(order, *this);
        // Step 4: Update the Order
        // Update order->quantity with the remaining quantity.
        order->quantity = remainingQty;
        // Step 5: Decide Fate (Market Maker vs Filled)
        if (remainingQty > 0){
            addRestingOrder(order);
        }
        else{
            delete order;
        }
    }

    Quantity OrderBook::match(Order* incomingOrder) {
        Quantity qty = incomingOrder->quantity;
        
        // Loop continuously until we run out of quantity OR run out of matches
        while (true) {
            // Step 1: Check if we are done (qty == 0). If so, break.
            if(qty == 0){
                break;
            }
            // Step 2: Get the best opposing price level.
            // Call 'getBestLevel()' using the incomingOrder's side.
            LimitLevel* bestLevel = getBestLevel(incomingOrder->side);
            // Step 3: Check if the book is empty.
            // If bestLevel is nullptr, we have no one to trade with. Break.
            if(bestLevel == nullptr){
                break;
            }
            // Step 4: Check if Prices Cross (The Deal Breaker)
            // If Buying: Is my Price >= Best Ask Price?
            // If Selling: Is my Price <= Best Bid Price?
            // If prices do NOT cross, we cannot trade. Break.
            if(incomingOrder->side == Side::Buy){
                if(incomingOrder->price < bestLevel->getPrice()){
                    break;
                }
            }
            else{ 
                if(incomingOrder->price > bestLevel->getPrice()){
                    break;
                }
            }

            // Step 5: Execution Loop (Consuming liquidity at this price)
            // Get the first order in the queue: bestLevel->getHead()
            Order* bookOrder = bestLevel->getHead();
            // While (there is a bookOrder AND we still have qty to fill):
            while(bookOrder != nullptr && qty > 0){
                // A. Calculate trade quantity: min(my_qty, their_qty)
                Quantity tradeQty = (qty < bookOrder->quantity) ? qty : bookOrder->quantity;
                // B. Print the Trade (or log it)
                std::cout << "Trade Executed: " << tradeQty << " @ " << bestLevel->getPrice() << endl;
                
                // C. Update quantities
                //    Decrease 'qty'
                //    Decrease 'bookOrder' quantity (call bookOrder->fill())
                qty -= tradeQty;
                bookOrder->fill(tradeQty);
                // D. Handle fully filled book orders
                //    If bookOrder is filled (qty == 0):
                //       1. Save 'bookOrder->next' (because we are about to delete current)
                //       2. Remove bookOrder from level (bestLevel->remove(bookOrder))
                //       3. Remove from orderLookup map
                //       4. 'delete' the bookOrder object
                //       5. Move pointer to the saved 'next' order
                Order* nextOrder = bookOrder->next;
                if(bookOrder->isFilled()){
                    bestLevel->remove(bookOrder);
                    orderLookup.erase(bookOrder->id);
                    delete bookOrder;
                }
                bookOrder = nextOrder;
            }
            // Step 6: Level Cleanup
            // If the 'bestLevel' is now empty (we ate all the orders there):
            //    1. Remove the price level from the Tree (bids or asks map).
            //    2. 'delete' the bestLevel object.
            if(bestLevel->isEmpty()){
                if(incomingOrder->side == Side::Buy){
                    asks.erase(bestLevel->getPrice());
                }
                else{
                    bids.erase(bestLevel->getPrice());
                }
                delete bestLevel;
            }
        }

        // Return the quantity that is LEFT OVER (could be 0).
        return qty;
    }

    void OrderBook::addRestingOrder(Order* order) {
        // Step 1: Add to HashMap
        // Store the order in 'orderLookup' so we can cancel it later in O(1).
        orderLookup[order->id] = order;
        // Step 2: Find or Create the LimitLevel
        // Depending on order->side (Buy vs Sell):
        //    Look up the price in the corresponding TreeMap (bids or asks).
        //    If it exists -> use it.
        //    If it DOES NOT exist -> 
        //         Create a 'new LimitLevel(order->price)'.
        //         Insert it into the TreeMap.
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
        // Step 3: Add to List
        level->append(order);
    }

    void OrderBook::cancelOrder(OrderId id) {
        // Step 1: Lookup
        // Find the ID in 'orderLookup'. If not found, return.
        auto it = orderLookup.find(id);
        if(it == orderLookup.end()){
            return;
        }
        Order* order = it->second;
        LimitLevel* parentLimit = order->parentLimit;
        // Step 2: Get the Order and Parent
        // Get the Order* from the map.
        // Get the LimitLevel* from order->parentLimit.

        // Step 3: Remove from List
        // Call parentLimit->remove(order).
        parentLimit->remove(order);
        // Step 4: Tree Cleanup (Crucial for memory/speed)
        // If parentLimit->isEmpty():
        //    Remove the price level from the TreeMap (bids or asks).
        //    'delete' the parentLimit object.
        if(parentLimit->isEmpty()){
            if(order->side == Side::Buy){
                bids.erase(parentLimit->getPrice());
            }
            else{
                asks.erase(parentLimit->getPrice());
            }
            delete parentLimit;
        }
        // Step 5: Memory Cleanup
        // Remove from 'orderLookup'.
        orderLookup.erase(id);
        // 'delete' the order object.
        delete order;
    }

    LimitLevel* OrderBook::getBestLevel(Side side) const {
        // Logic:
        // If Side is Buy:
        //    We want to match against SELLERS (Asks).
        //    Best Ask = Lowest Price = asks.begin() (since map is sorted 1..100)
        if(side == Side::Buy){
            if(!asks.empty()){
                return asks.begin()->second;
            }
            return nullptr;
        }
        // If Side is Sell:
        //    We want to match against BUYERS (Bids).
        //    Best Bid = Highest Price = bids.begin() (since map is sorted 100..1 due to std::greater)
        
        // Check if map is empty before accessing begin()!
        // Return nullptr if empty.
        if(side == Side::Sell){
            if(!bids.empty()){
                return bids.begin()->second;
            }
            return nullptr;
        }
        return nullptr; // Placeholder
    }
}