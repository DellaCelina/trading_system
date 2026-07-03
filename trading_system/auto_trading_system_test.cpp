#include "gmock/gmock.h"

#include <array>

#include "timer.h"
#include "stock_brocker_driver.h"
#include "auto_trading_system.h"

using namespace testing;

struct MockTimer : public TimerInterface {
    MOCK_METHOD(void, sleep, (int ms), (override));
};

struct MockStockBrokerDriver : public StockBrokerDriverInterface {
    MOCK_METHOD(void, login, (const std::string& id, const std::string& pass), (override));
    MOCK_METHOD(void, buy, (const std::string& stockCode, int price, int count), (override));
    MOCK_METHOD(void, sell, (const std::string& stockCode, int price, int count), (override));
    MOCK_METHOD(int, getPrice, (const std::string& stockCode), (override));
};

struct AutoTradingSystemTester : public Test {
    void SetUp() override {
        tradingSystem.setTimer(&timer);
        tradingSystem.selectStockBroker(&driver);
    }

    StrictMock<MockTimer> timer;
    StrictMock<MockStockBrokerDriver> driver;
    AutoTradingSystem tradingSystem;
};

TEST_F(AutoTradingSystemTester, buyNiceTimingSuccess) {
    const std::string stockCode = "code";
    const int totalMoney = 100;
    const std::array<int, 3> prices = { 2, 3, 4 };
    const int lastPrice = prices.back();
    const int buyCount = totalMoney / lastPrice;

    EXPECT_CALL(timer, sleep(200))
        .Times(2);

    auto&& priceExpect = EXPECT_CALL(driver, getPrice(stockCode));
    priceExpect.Times(3);

    for (auto price : prices) priceExpect.WillOnce(Return(price));

    EXPECT_CALL(driver, buy(stockCode, lastPrice, buyCount))
        .Times(1);

    tradingSystem.buyNiceTiming(stockCode, totalMoney);
}

TEST_F(AutoTradingSystemTester, buyNiceTimingFail) {
    const std::string stockCode = "code";
    const int totalMoney = 100;
    const std::array<int, 3> prices = { 4, 5, 5 };
    const int lastPrice = prices.back();

    EXPECT_CALL(timer, sleep(200))
        .Times(2);

    auto&& priceExpect = EXPECT_CALL(driver, getPrice(stockCode));
    priceExpect.Times(3);

    for (auto price : prices) priceExpect.WillOnce(Return(price));

    tradingSystem.buyNiceTiming(stockCode, totalMoney);
}

TEST_F(AutoTradingSystemTester, sellNiceTimingSuccess) {
    const std::string stockCode = "code";
    const int numberOfStock = 100;
    const std::array<int, 3> prices = { 4, 3, 2 };
    const int lastPrice = prices.back();

    EXPECT_CALL(timer, sleep(200))
        .Times(2);

    auto&& priceExpect = EXPECT_CALL(driver, getPrice(stockCode));
    priceExpect.Times(3);

    for (auto price : prices) priceExpect.WillOnce(Return(price));

    EXPECT_CALL(driver, sell(stockCode, lastPrice, numberOfStock))
        .Times(1);

    tradingSystem.sellNiceTiming(stockCode, numberOfStock);
}

TEST_F(AutoTradingSystemTester, sellNiceTimingFail) {
    const std::string stockCode = "code";
    const int numberOfStock = 100;
    const std::array<int, 3> prices = { 4, 3, 3 };
    const int lastPrice = prices.back();

    EXPECT_CALL(timer, sleep(200))
        .Times(2);

    auto&& priceExpect = EXPECT_CALL(driver, getPrice(stockCode));
    priceExpect.Times(3);

    for (auto price : prices) priceExpect.WillOnce(Return(price));

    tradingSystem.sellNiceTiming(stockCode, numberOfStock);
}
