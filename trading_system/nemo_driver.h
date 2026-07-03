#pragma once

#include "stock_brocker_driver.h"
#include "nemo_api.h"

struct NemoDriver : public StockBrokerDriverInterface {
    NemoDriver(NemoAPI* api) { nemo_api = api; }

    void login(const std::string& id, const std::string& pass) override {

        return nemo_api->certification(id, pass);
    }

    void buy(const std::string& stockCode, int price, int count) override {

        return nemo_api->purchasingStock(stockCode, price, count);

    }

    void sell(const std::string& stockCode, int price, int count) override {

        return nemo_api->sellingStock(stockCode, price, count);

    }

    int getPrice(const std::string& stockCode) override {

        return nemo_api->getMarketPrice(stockCode, 1);
    }

private:
    NemoAPI* nemo_api;
};
