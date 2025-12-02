#pragma once
#include <cstdint>

enum class Side{
    Buy,
    Sell
};

enum class OrderType {
    Market, // Executed immediately at best price
    Limit,  // Executed only at specific price or better
    GoodTillCancel,
    FillOrKill
};

using Price = double;
using Quantity = uint32_t;
using OrderId = uint64_t;