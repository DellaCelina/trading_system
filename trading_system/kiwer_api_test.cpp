#include "gmock/gmock.h"

#include <functional>
#include <numeric>
#include <format>

#include "kiwer_api.h"

using namespace testing;

using std::string;
using std::format;

struct KiwerAPITester : public Test {
    string captureStdout(std::function<void()> f) {
        internal::CaptureStdout();
        f();
        return internal::GetCapturedStdout();
    }

    KiwerAPI api;
};

TEST_F(KiwerAPITester, testLoginOutput) {
    const string ID = "TEST_ID";
    const string password = "TEST_PASS";
    const auto golden = format("{} login success\n", ID);

    auto output = captureStdout([&] {
        api.login(ID, password);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(KiwerAPITester, testBuyOutput) {
    const string stockCode = "STOCK_CODE";
    const int price = 10;
    const int count = 20;
    const auto golden = format("{} : Buy stock ( {} * {})\n", stockCode, price, count);

    auto output = captureStdout([&] {
        api.buy(stockCode, count, price);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(KiwerAPITester, testSellOutput) {
    const string stockCode = "STOCK_CODE";
    const int price = 10;
    const int count = 20;
    const auto golden = format("{} : Sell stock ( {} * {})\n", stockCode, price, count);

    auto output = captureStdout([&] {
        api.sell(stockCode, count, price);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(KiwerAPITester, testcurrentPriceOutput) {
    const string stockCode = "STOCK_CODE";
    const int minute = 10;

    auto price = api.currentPrice(stockCode);
    EXPECT_THAT(price, Ge(5000));
    EXPECT_THAT(price, Le(5900));
}
