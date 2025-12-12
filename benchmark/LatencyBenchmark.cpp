#include "core/OrderBook.hpp"
#include "types/Constants.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>

using namespace LOB;

int main() {
    const int NUM_ORDERS = 1e7;
    std::cout << "--- Benchmarking Optimized Engine (With Pool + Vector) ---" << std::endl;
    
    OrderBook book;
    
    // Random Generators
    std::mt19937 rng(42); 
    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<int> priceDist(90, 110);
    std::uniform_int_distribution<int> qtyDist(1, 100);
    // To store latencies
    std::vector<long long> latencies;
    latencies.reserve(NUM_ORDERS);

    try {
        for (int i = 0; i < NUM_ORDERS; ++i) {
            Side side = (sideDist(rng) == 0) ? Side::Buy : Side::Sell;
            Price price = (double)priceDist(rng);
            Quantity qty = qtyDist(rng);

            // Measure strictly the addOrder call
            auto t1 = std::chrono::high_resolution_clock::now();
            book.addOrder(i, side, price, qty);
            auto t2 = std::chrono::high_resolution_clock::now();

            long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            latencies.push_back(duration);
        }

        // --- Save to CSV ---
        std::cout << "Saving raw data to 'latencies.csv'..." << std::endl;
        std::ofstream outFile("/build/latencies.csv");
        outFile << "Latency_NS\n"; // Header
        long long sum = 0;
        for (long long lat : latencies) {
            outFile << lat << "\n";
            sum += lat;
        }
        outFile.close();

        std::cout << "Average Latency: " << (sum / NUM_ORDERS) << " ns" << std::endl;
        std::cout << "Done." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Benchmark failed: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}