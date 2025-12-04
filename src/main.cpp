#include "core/OrderBook.hpp"
#include "types/Constants.hpp"
#include <iostream>

using namespace LOB;

int main() {
    std::cout << "--- Initializing Limit Order Book ---" << std::endl;
    OrderBook book;

    // 1. Add some SELL orders (Asks)
    // Seller A wants to sell 100 @ $101
    // Seller B wants to sell 50  @ $102
    std::cout << "[Order 1] Sell 100 @ 101" << std::endl;
    book.addOrder(1, Side::Sell, 101.0, 100);

    std::cout << "[Order 2] Sell 50 @ 102" << std::endl;
    book.addOrder(2, Side::Sell, 102.0, 50);

    // 2. Add a BUY order that crosses the spread
    // Buyer C wants to buy 120 @ $103
    // Should match 100 @ 101 (Trade 1)
    // Should match 20  @ 102 (Trade 2)
    // Remaining 30 left on the book @ 102? No, 20 consumed. 
    // Wait, Seller B has 50. Buyer C takes 20. Seller B has 30 left.
    std::cout << "\n[Order 3] Buy 120 @ 103 (Aggressive)" << std::endl;
    book.addOrder(3, Side::Buy, 103.0, 120);

    // 3. Add a Passive BUY order
    // Buyer D wants to buy 10 @ 99 (No match, rests in book)
    std::cout << "\n[Order 4] Buy 10 @ 99 (Passive)" << std::endl;
    book.addOrder(4, Side::Buy, 99.0, 10);

    // 4. Cancel the Passive Order
    std::cout << "\n[Cancel] Cancelling Order 4" << std::endl;
    book.cancelOrder(4);

    std::cout << "\n--- End of Simulation ---" << std::endl;
    return 0;
}