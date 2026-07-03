#pragma once

#include <string>

struct StockBrokerDriverInterface {
    //로그인기능: login(id, pass)
    virtual void login(const std::string& id, const std::string& pass) = 0;

    //▪ 매수기능: buy(종목코드, 가격, 수량)
    virtual void buy(const std::string& stockCode, int price, int count) = 0;

    //▪ 매도기능: sell(종목코드, 가격, 수량)
    virtual void sell(const std::string& stockCode, int price, int count) = 0;

    //▪ 현재가확인기능: getPrice(종목코드)
    virtual int getPrice(const std::string& stockCode) = 0;
};
