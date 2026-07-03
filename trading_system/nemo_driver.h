#pragma once

#include "stock_brocker_driver.h"
#include "nemo_api.h"

struct NemoDriver : public StockBrokerDriverInterface {
    NemoDriver(NemoAPI* api) {}

    void login(const std::string& id, const std::string& pass) override {}

    void buy(const std::string& stockCode, int price, int count) override {}

    void sell(const std::string& stockCode, int price, int count) override {}

    int getPrice(const std::string& stockCode) override {
        return 0;
    }
};
