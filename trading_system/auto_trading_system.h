#pragma once

#include <string>
#include <memory>
#include <vector>

#include "stock_broker_driver.h"
#include "timer.h"
#include "order_scheduler.h"

#include "timing_strategy.h"

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

    // 추가 기능 #1
    //예약기능: scheduleOrder(주문, 실행시각)
    //• 주문을객체로만들어큐에저장한다.
    //• 시간이도달하면자동으로실행된다.
    //• 모든실행은자동으로로그를남긴다.
    //사용예
    //• 장시작직후자동매수
    //• 특정시각자동매도
    //Command 패턴으로 OCP 개발
    //주문을IOrderCommand로객체화한다.
    //BuyOrder / SellOrder 가 구현한다.
    //• OrderScheduler 가 예약 큐를 관리
    //특정시간혹은명령어시준비된명령들을자동실행
    //실행전후로자동로깅

    void setScheduler(OrderSchedulerInterface* scheduler) {}

    // Add to scheduler
    void scheduleOrder(time_t time, std::unique_ptr<OrderInterface>&& order) {} 

    // 특정시간시 준비된 명령들을 자동실행 (시간 순서로 실행)
    void startScheduledOrder() {
        // sleep은 timer 활용
    }

    // 명령어시 준비된명령들을자동실행 (전체 실행)
    void flushScheduledOrder() {}

    // 추가기능 #2
    // Auto Trading System – 매매 전략 교체 기능
    //ITimingStrategy 인터페이스 도입
    //• shouldBuy(priceHistory) : 매수 판단
    //• shouldSell(priceHistory) : 매도 판단
    //구현예시
    //• RisingTrendStrategy : 3회 연속 상승 추세
    //• MovingAverageStrategy : 이동평균선 돌파
    //• BreakoutStrategy : 직전 N개 고점 돌파
    //• Runtime 중 알고리즘 교체 가능한구조
    //AutoTradingSystem 이 Strategy 를 주입받음 (DI

    void setTimingStrategy(TimingStrategyInterface* strategy) {}

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
};
