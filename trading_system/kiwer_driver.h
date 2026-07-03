#pragma once

#include "stock_brocker_driver.h"
#include "kiwer_api.h"

struct KiwerDriver : public StockBrokerDriverInterface {
    KiwerDriver(KiwerAPI* api) {}

    void login(const std::string& id, const std::string& pass) override {}

    void buy(const std::string& stockCode, int price, int count) override {}

    void sell(const std::string& stockCode, int price, int count) override {}

    int getPrice(const std::string& stockCode) override {
        return 0;
    }
};
