#pragma once

#include "types/Order.hpp"
// We only forward declare OrderBook here to avoid circular includes in headers
namespace LOB {
    class OrderBook; 
}

namespace LOB {

    class MatchingEngine {
    public:
        // The main matching logic.
        // Takes an order and the book it belongs to.
        // Returns the remaining quantity of the incoming order.
        static Quantity match(Order* incomingOrder, OrderBook& book);
    };

}