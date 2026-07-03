#pragma once

#include <string>
#include <memory>
#include <vector>

#include "stock_broker_driver.h"
#include "timer.h"

class AutoTradingSystem {
public:

    //증권사선택기능: selectStockBrocker( )
    void selectStockBroker(StockBrokerDriverInterface* driver) {
        this->driver = driver;
    }

    //기능1 : buyNiceTiming(종목, 총 금액)
    //• 200ms 주기로 3회 가격을 읽고, 가격이올라가는추세인지파악한다.
    //• 가격이올라가는추세라면, 총금액을최대한사용하여최대수량만큼매수한다.
    //• 마지막에읽은가격으로매수한다.
    void buyNiceTiming(const std::string& stockCode, int totalMoney) {
        int firstPrice = driver->getPrice(stockCode);
        timer->sleep(200);
        int secondPrice = driver->getPrice(stockCode);
        timer->sleep(200);
        int thirdPrice = driver->getPrice(stockCode);

        bool isRising = firstPrice < secondPrice && secondPrice < thirdPrice;
        if (isRising) {
            int count = totalMoney / thirdPrice;
            driver->buy(stockCode, thirdPrice, count);
        }
    }

    //▪ 기능2 : sellNiceTiming(종목, 수량)
    //• 200ms 주기로 3회 가격을 읽고, 가격이 내려가는추세인지파악한다.
    //• 가격이내려가는추세라면, 사용자가설정한수량만큼주식을모두매도한다.
    //• 마지막에읽은가격으로매도한다
    void sellNiceTiming(const std::string& stockCode, int numberOfStock) {
        std::vector<int> prices = readPrices(stockCode, READ_COUNT);
        if (isDecreasing(prices)) {
            driver->sell(stockCode, prices.back(), numberOfStock);
        }
    }

private:
    std::vector<int> readPrices(const std::string& stockCode, int count) {
        std::vector<int> prices;
        for (int i = 0; i < count; i++) {
            prices.push_back(driver->getPrice(stockCode));
            if (i < count - 1) timer->sleep(READ_INTERVAL_MS);
        }
        return prices;
    }

    bool isDecreasing(const std::vector<int>& prices) {
        for (int i = 0; i < prices.size() - 1; i++) {
            if (prices[i] <= prices[i + 1]) return false;
        }
        return true;
    }

    static constexpr int READ_COUNT = 3;
    static constexpr int READ_INTERVAL_MS = 200;
    StockBrokerDriverInterface* driver = nullptr;
    TimerInterface* timer = nullptr;
};
