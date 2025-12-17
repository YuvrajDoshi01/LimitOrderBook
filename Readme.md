# Limit Order Book

![Language](https://img.shields.io/badge/language-C++20-00599C.svg)
![Build](https://img.shields.io/badge/build-CMake-brightgreen.svg)

## Project Description

This project is a high-performance **Limit Order Book (LOB)** implementation in C++. It serves as the core matching engine for a trading system, responsible for matching buy (bid) and sell (ask) orders based on price and time priority.

The Limit Order Book maintains a record of outstanding limit orders and executes trades when matching orders are found. This implementation focuses on **low latency** and **high throughput**, utilizing efficient data structures to manage the order queues and execute trades rapidly.

### Key Features
* **Order Matching Engine:** Matches incoming Buy/Sell orders using Price–Time priority.
* **Low-Latency Path:** Cache-friendly flat structures for price levels, pooled allocations, and minimal branching.
* **Order Management:** Add and cancel orders with O(1) lookup by ID.
* **Performance Benchmarking:** Built-in latency benchmark with CSV export and Python analysis.
* **Unit Testing:** Tests cover core matching and cancellation logic.

---

## Project Structure

The project is organized as follows:

* **`include/`**: Headers for core data structures (`types/Order.hpp`, `core/OrderBook.hpp`, `core/LimitLevel.hpp`).
* **`src/`**: Engine implementation (`core/OrderBook.cpp`, `core/MatchingEngine.cpp`, `core/LimitLevel.cpp`).
* **`benchmark/`**: Latency benchmark (`LatencyBenchmark.cpp`) and optional analysis script (`data_analysis.py`).
* **`tests/`**: Unit tests to verify functionality.
* **`CMakeLists.txt`**: Project build configuration.

---

## Technologies Used

* **Language:** C++20
* **Build System:** CMake
* **Testing:** (optional) GoogleTest
* **Benchmarking:** Custom micro-benchmark with CSV output

---

## Getting Started

Follow these instructions to compile and run the project on your local machine.

### Prerequisites

Ensure you have the following installed:
* **C++ Compiler** (GCC, Clang, or MSVC) supporting C++17 or later.
* **CMake** (Version 3.10 or higher).
* **Make** (or Ninja/Visual Studio depending on your OS).

### Installation

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/YuvrajDoshi01/LimitOrderBook.git
    cd LimitOrderBook
    ```

2.  **Create a build directory:**
    ```bash
    mkdir -p build && cd build
    ```

---

## Compilation & Build Instructions

This project uses **CMake** for building.

### Build (Release recommended)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j 8
```

---

## Usage

### Running the Demo App
From the `build/` directory:
```bash
./LOB
```

### Running Tests
To ensure the order book is working correctly, run the test suite:
```bash
cd tests
./limit_order_book_tests
# OR if using CTest
ctest --output-on-failure
```

### Running Benchmarks
From the `build/` directory, pass the number of orders as an argument (defaults to a large value if omitted):
```bash
./LOB_Benchmark 1000000
```
This writes `latencies.csv` in the build directory and prints the average latency. The benchmark uses a short warmup and batch timing to avoid 0 ns artifacts and stabilize measurements.

### Analyze Latency CSV (optional)
From the repo root:
```bash
python3 benchmark/data_analysis.py
```
This expects `build/latencies.csv` and will print summary stats and save a histogram image to `build/latency_histogram.png`.

---

## How It Works (Internal Logic)

The Order Book uses a **Price-Time Priority** matching algorithm:
1.  **Price Priority:** Buy orders with higher prices and Sell orders with lower prices are prioritized.
2.  **Time Priority:** If prices are equal, the order that arrived earlier is executed first (FIFO).

**Data Structures (current):**
- **Price Levels:** Flat, sorted vectors of `(price, level*)` for bids (descending) and asks (ascending), improving cache locality over tree-based maps.
- **Level Pooling:** `LimitLevel` objects are allocated from a pool to avoid frequent heap allocations and pointer chasing.
- **Orders:** Intrusive doubly-linked lists per level preserve FIFO; orders come from an object pool.
- **Lookup:** `std::vector<Order*>` for O(1) cancellation by `OrderId`.

**Matching Mechanics:**
- `MatchingEngine::match()` walks the best opposing level in FIFO order, updates quantities, and removes empty orders and levels.
- Partial fills correctly adjust per-level volume via `LimitLevel::decreaseVolume()`.
- Empty price levels are removed via `OrderBook::removeLevel()` and cached best pointers are refreshed.

**Public API Highlights:**
- `OrderBook::addOrder(id, side, price, qty)`
- `OrderBook::cancelOrder(id)`
- `OrderBook::getVolumeAtPrice(side, price)`
- `OrderBook::hasOrder(id)`

### Notes on Performance
- Build with `-DCMAKE_BUILD_TYPE=Release` for realistic numbers.
- The benchmark uses batched timing to mitigate timer resolution and call overhead.
- CSV latencies are per-order averages across small batches; use the Python script for percentiles.

