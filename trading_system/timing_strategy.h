#pragma once

#include <vector>

struct TimingStrategyInterface {
    virtual bool shouldBuy(const std::vector<int> priceHistory) = 0;
    virtual bool shouldSell(const std::vector<int> priceHistory) = 0;
};
