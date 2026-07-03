#include "gmock/gmock.h"

#include <array>
#include <format>

#include "timer.h"
#include "stock_broker_driver.h"
#include "auto_trading_system.h"

using namespace testing;

using std::string;
using std::format;

struct MockTimer : public TimerInterface {
    MOCK_METHOD(void, sleep, (int ms), (override));
};

struct MockStockBrokerDriver : public StockBrokerDriverInterface {
    MOCK_METHOD(void, login, (const string& id, const string& pass), (override));
    MOCK_METHOD(void, buy, (const string& stockCode, int price, int count), (override));
    MOCK_METHOD(void, sell, (const string& stockCode, int price, int count), (override));
    MOCK_METHOD(int, getPrice, (const string& stockCode), (override));
};

struct AutoTradingSystemTester : public Test {
    void SetUp() override {
        tradingSystem.setTimer(&timer);
        tradingSystem.selectStockBroker(&driver);
    }

    static constexpr int SLEEP_MS = 200;
    static constexpr int READ_COUNT = 3;

    StrictMock<MockTimer> timer;
    StrictMock<MockStockBrokerDriver> driver;
    AutoTradingSystem tradingSystem;
};

struct BuyNiceTimingTestParam {
    string stockCode;
    int totalMoney;
    std::array<int, AutoTradingSystemTester::READ_COUNT> prices;
};

std::ostream& operator<<(std::ostream& os, const  BuyNiceTimingTestParam& p) {
    os << format("BuyNiceTimingTestParam: stockCode({}) totalMoney({}) ", p.stockCode, p.totalMoney);
    os << "prices{ ";
    for (auto& v : p.prices) os << v << ", ";
    os << "}";
    return os;
}

struct AutoTradingSystemBuyNiceTimingTester :
    public AutoTradingSystemTester,
    public WithParamInterface<BuyNiceTimingTestParam> {
    void test(int buyCallCount) {
        auto&& p = GetParam();

        const int lastPrice = p.prices.back();
        const int buyCount = p.totalMoney / lastPrice;

        EXPECT_CALL(timer, sleep(SLEEP_MS))
            .Times(READ_COUNT - 1);

        auto&& priceExpect = EXPECT_CALL(driver, getPrice(p.stockCode));
        priceExpect.Times(READ_COUNT);

        for (auto price : p.prices) priceExpect.WillOnce(Return(price));

        EXPECT_CALL(driver, buy(p.stockCode, lastPrice, buyCount))
            .Times(buyCallCount);

        tradingSystem.buyNiceTiming(p.stockCode, p.totalMoney);
    }
};

struct AutoTradingSystemBuyNiceTimingSuccessTester : public AutoTradingSystemBuyNiceTimingTester {};
TEST_P(AutoTradingSystemBuyNiceTimingSuccessTester, testSuccessCases) {
    test(1);
}

INSTANTIATE_TEST_SUITE_P(
    AutoTradingSystemBuyNiceTimingSuccessTest,
    AutoTradingSystemBuyNiceTimingSuccessTester,
    Values(
        BuyNiceTimingTestParam {
            .stockCode = "STOCK_CODE",
            .totalMoney = 100,
            .prices = { 2, 3, 4 }
        }
    )
);

struct AutoTradingSystemBuyNiceTimingFailTester : public AutoTradingSystemBuyNiceTimingTester {};
TEST_P(AutoTradingSystemBuyNiceTimingFailTester, testFailCases) {
    test(0);
}

INSTANTIATE_TEST_SUITE_P(
    AutoTradingSystemBuyNiceTimingFailTest,
    AutoTradingSystemBuyNiceTimingFailTester,
    Values(
        BuyNiceTimingTestParam {
            .stockCode = "STOCK_CODE",
            .totalMoney = 100,
            .prices = { 2, 3, 3 }
        }
    )
);

struct SellNiceTimingTestParam {
    string stockCode;
    int numberOfStock;
    std::array<int, AutoTradingSystemTester::READ_COUNT> prices;
};

std::ostream& operator<<(std::ostream& os, const  SellNiceTimingTestParam& p) {
    os << format("SellNiceTimingTestParam: stockCode({}) totalMoney({}) ", p.stockCode, p.numberOfStock);
    os << "prices{ ";
    for (auto& v : p.prices) os << v << ", ";
    os << "}";
    return os;
}

struct AutoTradingSystemSellNiceTimingTester :
    public AutoTradingSystemTester,
    public WithParamInterface<SellNiceTimingTestParam> {
    void test(int sellCallCount) {
        auto&& p = GetParam();
        const int lastPrice = p.prices.back();

        EXPECT_CALL(timer, sleep(SLEEP_MS))
            .Times(READ_COUNT - 1);

        auto&& priceExpect = EXPECT_CALL(driver, getPrice(p.stockCode));
        priceExpect.Times(READ_COUNT);

        for (auto price : p.prices) priceExpect.WillOnce(Return(price));

        EXPECT_CALL(driver, sell(p.stockCode, lastPrice, p.numberOfStock))
            .Times(sellCallCount);

        tradingSystem.sellNiceTiming(p.stockCode, p.numberOfStock);
    }
};

struct AutoTradingSystemSellNiceTimingSuccessTester : public AutoTradingSystemSellNiceTimingTester {};
TEST_P(AutoTradingSystemSellNiceTimingSuccessTester, testSuccessCases) {
    test(1);
}

INSTANTIATE_TEST_SUITE_P(
    AutoTradingSystemSellNiceTimingSuccessTest,
    AutoTradingSystemSellNiceTimingSuccessTester,
    Values(
        SellNiceTimingTestParam {
            .stockCode = "STOCK_CODE",
            .numberOfStock = 100,
            .prices = { 4, 3, 2 },
        }
    )
);

struct AutoTradingSystemSellNiceTimingFailTester : public AutoTradingSystemSellNiceTimingTester {};
TEST_P(AutoTradingSystemSellNiceTimingFailTester, testFailCases) {
    test(0);
}

INSTANTIATE_TEST_SUITE_P(
    AutoTradingSystemSellNiceTimingFailTest,
    AutoTradingSystemSellNiceTimingFailTester,
    Values(
        SellNiceTimingTestParam {
            .stockCode = "STOCK_CODE",
            .numberOfStock = 100,
            .prices = { 4, 3, 3 },
        }
    )
);
