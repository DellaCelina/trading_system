#pragma once

#include <string>

#include "stock_brocker_driver.h"
#include "timer.h"

class AutoTradingSystem {
public:
    //Dependency Injection for timer
    void setTimer(TimerInterface* timer) {}

    //증권사선택기능: selectStockBrocker( )
    void selectStockBroker(StockBrokerDriverInterface* driver) {}

    //기능1 : buyNiceTiming(종목, 총 금액)
    //• 200ms 주기로 3회 가격을 읽고, 가격이올라가는추세인지파악한다.
    //• 가격이올라가는추세라면, 총금액을최대한사용하여최대수량만큼매수한다.
    //• 마지막에읽은가격으로매수한다.
    void buyNiceTiming(const std::string& stockCode, int totalMoney) {}

    //▪ 기능2 : sellNiceTiming(종목, 수량)
    //• 200ms 주기로 3회 가격을 읽고, 가격이 내려가는추세인지파악한다.
    //• 가격이내려가는추세라면, 사용자가설정한수량만큼주식을모두매도한다.
    //• 마지막에읽은가격으로매도한다
    void sellNiceTiming(const std::string& stockCode, int numberOfStock) {}

private:
    StockBrokerDriverInterface* driver = nullptr;
    TimerInterface* timer = nullptr;
};
