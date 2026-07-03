#include "gmock/gmock.h"

#include "stock_broker_driver.h"
#include "buy_order.h"
#include "sell_order.h"

using namespace testing;

struct MockStockBrokerDriver : public StockBrokerDriverInterface {
    MOCK_METHOD(void, login, (const std::string& id, const std::string& pass), (override));
    MOCK_METHOD(void, buy, (const std::string& stockCode, int price, int count), (override));
    MOCK_METHOD(void, sell, (const std::string& stockCode, int price, int count), (override));
    MOCK_METHOD(int, getPrice, (const std::string& stockCode), (override));
};

struct OrderCommandTester : public Test {
    StrictMock<MockStockBrokerDriver> driver;
};

TEST_F(OrderCommandTester, buyOrderExecuteCallsDriverBuy) {
    const std::string stockCode = "code";
    const int price = 100;
    const int count = 3;

    EXPECT_CALL(driver, buy(stockCode, price, count))
        .Times(1);

    BuyOrder order(&driver, stockCode, price, count);
    order.execute();
}

TEST_F(OrderCommandTester, sellOrderExecuteCallsDriverSell) {
    const std::string stockCode = "code";
    const int price = 100;
    const int count = 3;

    EXPECT_CALL(driver, sell(stockCode, price, count))
        .Times(1);

    SellOrder order(&driver, stockCode, price, count);
    order.execute();
}
