# Limit Order Book

![Language](https://img.shields.io/badge/language-C++-00599C.svg)
![Build](https://img.shields.io/badge/build-CMake-brightgreen.svg)

## Project Description

This project is a high-performance **Limit Order Book (LOB)** implementation in C++. It serves as the core matching engine for a trading system, responsible for matching buy (bid) and sell (ask) orders based on price and time priority.

The Limit Order Book maintains a record of outstanding limit orders and executes trades when matching orders are found. This implementation focuses on **low latency** and **high throughput**, utilizing efficient data structures to manage the order queues and execute trades rapidly.

### Key Features
* **Order Matching Engine:** Automatically matches incoming Buy and Sell orders using the Price-Time Priority algorithm.
* **Order Types:** Supports **Limit Orders** (buy/sell at a specific price) and **Market Orders** (buy/sell immediately at the best available price).
* **Order Management:** Capabilities to **Add**, **Cancel**, and **Modify** existing orders.
* **Performance Benchmarking:** Includes a dedicated benchmark suite to measure latency and throughput under high-load scenarios.
* **Unit Testing:** Comprehensive test suite ensuring the correctness of order matching logic and edge case handling.

---

## Project Structure

The project is organized as follows:

* **`include/`**: Header files defining the core data structures (e.g., `Order`, `OrderBook`, `Trade`).
* **`src/`**: Source code implementing the order book logic and matching engine.
* **`tests/`**: Unit tests to verify functionality.
* **`benchmark/`**: Performance tests to measure execution speed and latency.
* **`CMakeLists.txt`**: Configuration file for the CMake build system.

---

## Technologies Used

* **Language:** C++ (C++17/20 standard recommended)
* **Build System:** CMake
* **Testing:** GoogleTest (GTest)
* **Benchmarking:** Google Benchmark

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
    It is best practice to keep build files separate from source code.
    ```bash
    mkdir build
    cd build
    ```

---

## Compilation & Build Instructions

This project uses **CMake** for building.

### Step 1: Configure the Project
Run the cmake command to generate the makefiles.
```bash
cmake ..
```
*Note: If you want to build in Release mode for maximum performance (recommended for benchmarking), use:*
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Step 2: Compile the Code
Build the project executables.
```bash
make
```
*(On Windows with Visual Studio, you would open the generated `.sln` file or use `cmake --build .`)*

---

## 💻 Usage

### Running the Application
After building, the main executable will be located in the `build` (or `build/bin`) directory.
```bash
./LimitOrderBook
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
To see the performance metrics:
```bash
cd benchmark
./limit_order_book_benchmark
```

---

## How It Works (Internal Logic)

The Order Book uses a **Price-Time Priority** matching algorithm:
1.  **Price Priority:** Buy orders with higher prices and Sell orders with lower prices are prioritized.
2.  **Time Priority:** If prices are equal, the order that arrived earlier is executed first (FIFO).

**Data Structures:**
* **Bids (Buy Orders):** Stored in a max-heap or sorted map (descending order) to quickly access the highest buy price.
* **Asks (Sell Orders):** Stored in a min-heap or sorted map (ascending order) to quickly access the lowest sell price.
* **Order Map:** A hash map is often used to look up orders by ID O(1) for quick cancellations. [Will be implementing the Vector version soon to make the process cachable]

