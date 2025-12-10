#pragma once

#include "types/Order.hpp"
#include <vector>
#include <stdexcept>

namespace LOB {

    class OrderPool {
    public:
        // Initialize pool with a fixed size (e.g., 100,000 orders)
        explicit OrderPool(size_t size) {
            // 1. Reserve memory for the block
            pool.reserve(size);
            
            // 2. Initialize the objects
            // We create 0-initialized orders just to fill the vector
            for (size_t i = 0; i < size; ++i) {
                pool.emplace_back(0, Side::Buy, 0, 0);
            }

            // 3. Populate the free list
            // Initially, every order in the pool is free.
            freeList.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                // Point to the address of the order inside the vector
                freeList.push_back(&pool[i]);
            }
        }

        // Get a fresh object from the pool (Replaces 'new')
        Order* allocate(OrderId id, Side side, Price price, Quantity quantity) {
            if (freeList.empty()) {
                throw std::runtime_error("OrderPool exhausted! Increase pool size.");
            }

            // LIFO (Last-In-First-Out) is faster due to CPU cache locality
            Order* order = freeList.back();
            freeList.pop_back();

            // Reset the old object with new data
            // You need to ensure Order class has a reset() method!
            order->id = id;
            order->side = side;
            order->price = price;
            order->quantity = quantity;
            order->initialQuantity = quantity;
            order->next = nullptr;
            order->prev = nullptr;
            order->parentLimit = nullptr;

            return order;
        }

        // Return an object to the pool (Replaces 'delete')
        void deallocate(Order* order) {
            freeList.push_back(order);
        }

    private:
        // The actual memory storage (Contiguous block)
        std::vector<Order> pool;

        // The list of available slots (Pointers to the pool)
        std::vector<Order*> freeList;
    };
}