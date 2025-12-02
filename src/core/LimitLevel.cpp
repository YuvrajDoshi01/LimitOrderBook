#include "core/LimitLevel.hpp"

namespace LOB {

    LimitLevel::LimitLevel(Price price) 
        : price(price), totalVolume(0), head(nullptr), tail(nullptr) {}

    void LimitLevel::append(Order* order) {
        // 1. Sanity check: If order is null, return.
        if(order == nullptr) {
            return;
        }

        // 2. Set the order's parent pointer to 'this' (so it knows where it lives).
        order->parentLimit = this;

        // 3. Update the totalVolume of this level.
        totalVolume += order->quantity;

        // 4. Linked List Logic:
        // Case A: The list is empty (head is null).
        //    - Set head = order
        //    - Set tail = order
        //    - Set order->next = nullptr
        //    - Set order->prev = nullptr
        if(head == nullptr){
            head = order;
            tail = order;
            order-> next = nullptr;
            order-> prev = nullptr;
        }   
        else{
            tail -> next = order;
            order -> prev = tail;
            order -> next = nullptr;
            tail = order;
        }
        return;
    }

    void LimitLevel::remove(Order* order) {
        // 1. Sanity check: If order is null, return.
        if(order == nullptr){
            return;
        }
        // 2. Decrease totalVolume by order->quantity.
        totalVolume -= order -> quantity;
        
        // 3. Update Pointers (The Surgical Removal):
       
        // Handle the 'prev' pointer
        if (order-> prev != nullptr){
            order -> prev -> next = order -> next;
        }
        else{
            head = order -> next;
        }
        
        // Handle the 'next' pointer
        if (order -> next != nullptr){
            order -> next -> prev = order -> prev;
        }
        else{
            tail = order -> prev;
        }

        // 4. Cleanup: 
        order -> parentLimit = nullptr;
        order -> next = nullptr;
        order -> prev = nullptr;
        
        return;
    }
}