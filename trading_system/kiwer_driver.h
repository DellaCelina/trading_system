#pragma once

#include "stock_broker_driver.h"
#include "kiwer_api.h"

struct KiwerDriver : public StockBrokerDriverInterface {
    KiwerDriver(KiwerAPI* api) { kiwer_api = api;}

    void login(const std::string& id, const std::string& pass) override {
        return kiwer_api->login(id, pass);
    }

    void buy(const std::string& stockCode, int price, int count) override {
        return kiwer_api->buy(stockCode, count, price);
    }

    void sell(const std::string& stockCode, int price, int count) override {
        return kiwer_api->sell(stockCode, count, price);
    }

    int getPrice(const std::string& stockCode) override {
        return kiwer_api->currentPrice(stockCode);
    }

private:
    KiwerAPI* kiwer_api;
};
