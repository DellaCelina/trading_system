#pragma once

#include <string>
#include <memory>
#include <vector>

#include "stock_brocker_driver.h"
#include "timer.h"

class AutoTradingSystem {
    public:
    //로그인기능: login(id, pass)
    void login(const std::string& id, const std::string& pass) {}

    //▪ 매수기능: buy(종목코드, 가격, 수량)
    void buy(const std::string& stockCode, int price, int count) {}

    //▪ 매도기능: sell(종목코드, 가격, 수량)
    void sell(const std::string& stockCode, int price, int count) {}

    //▪ 현재가확인기능: getPrice(종목코드)
    int getPrice(const std::string& stockCode) {}

    //증권사선택기능: selectStockBrocker( )
    void selectStockBroker(StockBrokerDriverInterface* driver) {
        this->driver = driver;
    }

    //기능1 : buyNiceTiming(종목, 총 금액)
    //• 200ms 주기로 3회 가격을 읽고, 가격이올라가는추세인지파악한다.
    //• 가격이올라가는추세라면, 총금액을최대한사용하여최대수량만큼매수한다.
    //• 마지막에읽은가격으로매수한다.
    void buyNiceTiming(const std::string& stockCode, int totalMoney) {}

    //▪ 기능2 : sellNiceTiming(종목, 수량)
    //• 200ms 주기로 3회 가격을 읽고, 가격이 내려가는추세인지파악한다.
    //• 가격이내려가는추세라면, 사용자가설정한수량만큼주식을모두매도한다.
    //• 마지막에읽은가격으로매도한다
    void sellNiceTiming(const std::string& stockCode, int numberOfStock) {
        std::vector<int> prices = readPricesThreeTimes(stockCode);
        if (isDecreasing(prices)) {
            driver->sell(stockCode, prices.back(), numberOfStock);
        }
    }

    //Dependency Injection for timer
    void setTimer(TimerInterface* timer) {
        this->timer = timer;
    }

private:
    std::vector<int> readPricesThreeTimes(const std::string& stockCode) {
        std::vector<int> prices;
        prices.push_back(driver->getPrice(stockCode));
        timer->sleep(READ_INTERVAL_MS);
        prices.push_back(driver->getPrice(stockCode));
        timer->sleep(READ_INTERVAL_MS);
        prices.push_back(driver->getPrice(stockCode));
        return prices;
    }

    bool isDecreasing(const std::vector<int>& prices) {
        return prices[0] > prices[1] && prices[1] > prices[2];
    }

    static constexpr int READ_INTERVAL_MS = 200;
    StockBrokerDriverInterface* driver = nullptr;
    TimerInterface* timer = nullptr;
};
