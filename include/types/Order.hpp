#pragma once
#include "Constants.hpp"
#include <limits>

// Forward declaration to avoid circular dependency
namespace LOB {
    class LimitLevel;
}

struct Order{
    OrderId id;
    Side side;
    Price price;
    Quantity quantity;
    Quantity initialQuantity;
    
    // --- System Pointers (The "Intrusive" Linked List) ---
    // These pointers allow the Order to act as a node in a list.
    // By storing them here, we avoid allocating a separate "Node" object.
    Order* next = nullptr; // Pointer to the next order in the list
    Order* prev = nullptr; // Pointer to the previous order in the list

    // Pointer to the Price Level this order belongs to.
    // Needed so we can quickly update the limit's volume when this order is cancelled.
    LOB::LimitLevel* parentLimit = nullptr;

    Order(OrderId id, Side side, Price price, Quantity quantity)
        : id(id), side(side), price(price), 
          quantity(quantity), initialQuantity(quantity) {}

    bool isFilled() const {
        return quantity == 0;
    }
    
    void fill(Quantity qty) {
        int x = ((qty > quantity) ? quantity = 0 : quantity -= qty);
        return;
    }    
};