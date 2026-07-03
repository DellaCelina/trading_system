#include "gmock/gmock.h"

#include <array>
#include <format>
#include <vector>

#include "order.h"
#include "timer.h"
#include "stock_broker_driver.h"
#include "auto_trading_system.h"
#include "timing_strategy.h"
#include "order_scheduler.h"

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

struct MockTimingStrategy : public TimingStrategyInterface {
    MOCK_METHOD(size_t, pricesNeededCount, (), (override));
    MOCK_METHOD(bool, shouldBuy, (const std::vector<int> priceHistory), (override));
    MOCK_METHOD(bool, shouldSell, (const std::vector<int> priceHistory), (override));
};

struct MockOrder : public OrderInterface {
    MOCK_METHOD(void, execute, (), (override));
};

struct MockOrderScheduler : public OrderSchedulerInterface {
    MOCK_METHOD(void, add, (time_t time, OrderInterface* order), (override));
    MOCK_METHOD(time_t, execute, (), (override));
};

struct AutoTradingSystemTester : public Test {
    void SetUp() override {
        tradingSystem.setTimer(&timer);
        tradingSystem.selectStockBroker(&driver);
        tradingSystem.setTimingStrategy(&timingStrategy);
        tradingSystem.setScheduler(&scheduler);

        ON_CALL(timingStrategy, pricesNeededCount())
            .WillByDefault(Return(READ_COUNT));
    }

    std::string captureStdout(std::function<void()> f) {
        internal::CaptureStdout();
        f();
        return internal::GetCapturedStdout();
    }

    static constexpr int SLEEP_MS = 200;
    static constexpr int READ_COUNT = 3;

    StrictMock<MockTimer> timer;
    StrictMock<MockStockBrokerDriver> driver;
    StrictMock<MockTimingStrategy> timingStrategy;
    StrictMock<MockOrderScheduler> scheduler;
    AutoTradingSystem tradingSystem;
};

struct BuyNiceTimingTestParam {
    string stockCode;
    int totalMoney;
    std::vector<int> prices;
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
    void test(bool needToBuy) {
        auto&& p = GetParam();

        const int lastPrice = p.prices.back();
        const int buyCount = p.totalMoney / lastPrice;

        EXPECT_CALL(timer, sleep(SLEEP_MS))
            .Times(READ_COUNT - 1);

        auto&& priceExpect = EXPECT_CALL(driver, getPrice(p.stockCode));
        priceExpect.Times(READ_COUNT);
        for (auto price : p.prices) priceExpect.WillOnce(Return(price));

        EXPECT_CALL(timingStrategy, shouldBuy(p.prices))
            .Times(1)
            .WillOnce(Return(needToBuy));

        EXPECT_CALL(driver, buy(p.stockCode, lastPrice, buyCount))
            .Times(needToBuy ? 1 : 0);

        tradingSystem.buyNiceTiming(p.stockCode, p.totalMoney);
    }
};

struct AutoTradingSystemBuyNiceTimingSuccessTester : public AutoTradingSystemBuyNiceTimingTester {};
TEST_P(AutoTradingSystemBuyNiceTimingSuccessTester, testSuccessCases) {
    test(true);
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
    test(false);
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
    std::vector<int> prices;
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
    void test(bool needToSell) {
        auto&& p = GetParam();
        const int lastPrice = p.prices.back();

        EXPECT_CALL(timer, sleep(SLEEP_MS))
            .Times(READ_COUNT - 1);

        auto&& priceExpect = EXPECT_CALL(driver, getPrice(p.stockCode));
        priceExpect.Times(READ_COUNT);

        for (auto price : p.prices) priceExpect.WillOnce(Return(price));

        EXPECT_CALL(timingStrategy, shouldSell(p.prices))
            .Times(1)
            .WillOnce(Return(needToSell));

        EXPECT_CALL(driver, sell(p.stockCode, lastPrice, p.numberOfStock))
            .Times(needToSell ? 1 : 0);

        tradingSystem.sellNiceTiming(p.stockCode, p.numberOfStock);
    }
};

struct AutoTradingSystemSellNiceTimingSuccessTester : public AutoTradingSystemSellNiceTimingTester {};
TEST_P(AutoTradingSystemSellNiceTimingSuccessTester, testSuccessCases) {
    test(true);
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
    test(false);
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

TEST_F(AutoTradingSystemTester, scheduleOrderTest) {
    const time_t time = 10;

    MockOrder testOrder;
    EXPECT_CALL(scheduler, add(time, &testOrder))
        .Times(1);

    tradingSystem.scheduleOrder(time, &testOrder);
}

TEST_F(AutoTradingSystemTester, startScheduleOrderTest) {
    EXPECT_CALL(scheduler, execute())
        .Times(3)
        .WillOnce(Return(10))
        .WillOnce(Return(14))
        .WillOnce(Return(-1));

    auto output = captureStdout([&] {
        tradingSystem.startScheduledOrder();
        });
    EXPECT_THAT(output.size(), Gt(0));
}

TEST_F(AutoTradingSystemTester, flushScheduleOrderTest) {
    EXPECT_CALL(scheduler, execute())
        .Times(3)
        .WillOnce(Return(10))
        .WillOnce(Return(14))
        .WillOnce(Return(-1));

    auto output = captureStdout([&] {
        tradingSystem.flushScheduledOrder();
        });
    EXPECT_THAT(output.size(), Gt(0));
}
