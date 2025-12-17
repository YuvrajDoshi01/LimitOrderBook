#pragma once

#include "core/LimitLevel.hpp"
#include <deque>
#include <stdexcept>

namespace LOB {

class LimitLevelPool {
public:
    explicit LimitLevelPool(size_t size = 1024) {
        freeList.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            pool.emplace_back(0.0);
            freeList.push_back(&pool.back());
        }
    }

    LimitLevel* allocate(Price price) {
        if (freeList.empty()) {
            // Grow by doubling to amortize allocations; deque keeps pointers stable
            size_t oldSize = pool.size();
            size_t newSize = oldSize ? oldSize * 2 : 1024;
            freeList.reserve(freeList.size() + (newSize - oldSize));
            for (size_t i = oldSize; i < newSize; ++i) {
                pool.emplace_back(0.0);
                freeList.push_back(&pool.back());
            }
        }
        LimitLevel* level = freeList.back();
        freeList.pop_back();
        // Reinitialize
        *level = LimitLevel(price);
        return level;
    }

    void deallocate(LimitLevel* level) {
        freeList.push_back(level);
    }

private:
    std::deque<LimitLevel> pool;
    std::vector<LimitLevel*> freeList;
};

}
