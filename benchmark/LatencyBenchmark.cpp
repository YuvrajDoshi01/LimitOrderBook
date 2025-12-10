#include "core/OrderBook.hpp"
#include "types/Constants.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>

using namespace LOB;

int main() {
    // 1. Setup - 1 Million Orders
    const int NUM_ORDERS = 1000000;
    std::cout << "--- Benchmarking Optimized Engine (With Object Pool) ---" << std::endl;
    
    // Note: Ensure your OrderBook constructor initializes the pool 
    // with enough size (e.g., 1.2 Million) to handle these orders.
    OrderBook book;
    
    // Random Generators (Fixed Seed for consistency)
    std::mt19937 rng(42); 
    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<int> priceDist(90, 110);
    std::uniform_int_distribution<int> qtyDist(1, 100);

    try {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < NUM_ORDERS; ++i) {
            Side side = (sideDist(rng) == 0) ? Side::Buy : Side::Sell;
            Price price = (double)priceDist(rng);
            Quantity qty = qtyDist(rng);

            // This now uses the Object Pool internally
            book.addOrder(i, side, price, qty);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

        // 3. Report Results
        double seconds = duration / 1e9;
        long long throughput = (long long)(NUM_ORDERS / seconds);

        std::cout << "Total Time: " << seconds << " seconds" << std::endl;
        std::cout << "Throughput: " << throughput << " orders/sec" << std::endl;
        std::cout << "Avg Latency: " << (duration / NUM_ORDERS) << " ns/order" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] Benchmark failed: " << e.what() << std::endl;
        std::cerr << "Hint: Did you forget to increase pool size or fix a 'delete'?" << std::endl;
        return -1;
    }

    return 0;
}