#include "core/OrderBook.hpp"
#include "types/Constants.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <random>

using namespace LOB;

int main(int argc, char** argv) {
    int NUM_ORDERS = 1e8;
    if (argc > 1) {
        try {
            long long v = std::stoll(argv[1]);
            if (v > 0 && v <= 1000000000LL) NUM_ORDERS = static_cast<int>(v);
        } catch (...) {}
    }
    std::cout << "--- Benchmarking Optimized Engine (With Pool + Vector) ---\n"
              << "Orders: " << NUM_ORDERS << std::endl;
    
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
        // Warm-up to stabilize caches and predictors (up to 1000 orders)
        for (int w = 0; w < std::min(1000, NUM_ORDERS); ++w) {
            Side side = (sideDist(rng) == 0) ? Side::Buy : Side::Sell;
            Price price = (double)priceDist(rng);
            Quantity qty = qtyDist(rng);
            book.addOrder(1'000'000'000ULL + w, side, price, qty);
        }

        // Batch timing to reduce 0 ns artifacts and clock noise
        using clock = std::chrono::steady_clock;
        constexpr int BATCH = 256;
        int placed = 0;
        while (placed < NUM_ORDERS) {
            int take = std::min(BATCH, NUM_ORDERS - placed);
            auto t1 = clock::now();
            for (int j = 0; j < take; ++j) {
                Side side = (sideDist(rng) == 0) ? Side::Buy : Side::Sell;
                Price price = (double)priceDist(rng);
                Quantity qty = qtyDist(rng);
                book.addOrder(placed + j, side, price, qty);
            }
            auto t2 = clock::now();
            long long batch_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            long long per_order = take > 0 ? batch_ns / take : 0;
            for (int j = 0; j < take; ++j) latencies.push_back(per_order);
            placed += take;
        }

        // --- Save to CSV ---
        std::cout << "Saving raw data to 'latencies.csv'..." << std::endl;
        std::ofstream outFile("latencies.csv");
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