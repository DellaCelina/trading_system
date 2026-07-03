#pragma once

#include <string>

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
    int getPrice(const std::string& stockCode) {
        return 0;
    }

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

    //Dependency Injection for timer
    void setTimer(TimerInterface* timer) {}

private:
    StockBrokerDriverInterface* driver = nullptr;
    TimerInterface* timer = nullptr;
};
