#pragma once

#include "types/Order.hpp"
#include <iostream>

namespace LOB {

    class LimitLevel {
    public:
        // --- Constructor ---
        explicit LimitLevel(Price price);

        // --- Core Operations ---
        
        // Appends an order to the end of the list (Time Priority)
        void append(Order* order);

        // Removes an order from the list (Cancellation or Execution)
        void remove(Order* order);

        // Decreases tracked volume when an order is partially filled
        void decreaseVolume(Quantity qty);

        // --- Getters ---
        Price getPrice() const { return price; }
        Quantity getVolume() const { return totalVolume; }
        bool isEmpty() const { return head == nullptr; }

        // Peek at the first order (for execution)
        Order* getHead() const { return head; }
        Order* getTail() const { return tail; }

    private:
        Price price;
        Quantity totalVolume; // Total quantity of all orders at this level

        // Pointers to the Linked List
        Order* head;
        Order* tail;
    };

}