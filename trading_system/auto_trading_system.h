#pragma once

#include <string>
#include <memory>
#include <vector>

#include "stock_broker_driver.h"
#include "timer.h"
#include "order_command.h"
#include "order_scheduler.h"

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
        std::vector<int> prices = readPrices(stockCode, READ_COUNT);
        if (isIncreasing(prices)) {
            int lastPrice = prices.back();
            int count = totalMoney / lastPrice;
            driver->buy(stockCode, lastPrice, count);
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

    //Dependency Injection for timer
    void setTimer(TimerInterface* timer) {
        this->timer = timer;
    }

    //Dependency Injection for scheduler
    void setScheduler(OrderSchedulerInterface* scheduler) {
        this->scheduler = scheduler;
    }

    //예약기능 : scheduleOrder(주문, 실행시각)
    void scheduleOrder(std::shared_ptr<IOrderCommand> order, long long executeAt) {
        scheduler->scheduleOrder(order, executeAt);
    }

    //시간이 도달한 예약 주문을 실행한다.
    void startScheduledOrder() {
        scheduler->startScheduledOrder();
    }

    //남은 예약 주문을 즉시 모두 실행한다.
    void flushScheduledOrder() {
        scheduler->flushScheduledOrder();
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

    bool isIncreasing(const std::vector<int>& prices) {
        for (int i = 0; i < prices.size() - 1; i++) {
            if (prices[i] >= prices[i + 1]) return false;
        }
        return true;
    }

    static constexpr int READ_COUNT = 3;
    static constexpr int READ_INTERVAL_MS = 200;
    StockBrokerDriverInterface* driver = nullptr;
    TimerInterface* timer = nullptr;
    OrderSchedulerInterface* scheduler = nullptr;
};
