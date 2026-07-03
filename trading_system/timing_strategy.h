#pragma once

#include <vector>

struct TimingStrategyInterface {
    // 필요한 Price 갯수
    virtual size_t pricesNeededCount() = 0;
    virtual bool shouldBuy(const std::vector<int> priceHistory) = 0;
    virtual bool shouldSell(const std::vector<int> priceHistory) = 0;
};
