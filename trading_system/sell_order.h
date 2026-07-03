#pragma once

#include <string>

#include "order_command.h"
#include "stock_broker_driver.h"

class SellOrder : public IOrderCommand {
public:
    SellOrder(StockBrokerDriverInterface* driver, const std::string& stockCode, int price, int count)
        : driver(driver), stockCode(stockCode), price(price), count(count) {}

    void execute() override {
        driver->sell(stockCode, price, count);
    }

private:
    StockBrokerDriverInterface* driver;
    std::string stockCode;
    int price;
    int count;
};
