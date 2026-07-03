#include "gmock/gmock.h"

#include <functional>
#include <numeric>
#include <format>

#include "nemo_api.h"

using namespace testing;

using std::string;
using std::format;

struct NemoAPITester : public Test {
    string captureStdout(std::function<void()> f) {
        internal::CaptureStdout();
        f();
        return internal::GetCapturedStdout();
    }

    NemoAPI api;
};

TEST_F(NemoAPITester, testCertificationOutput) {
    const string ID = "TEST_ID";
    const string pass = "TEST_PASS";
    const auto golden = format("[NEMO]{} login GOOD\n", ID);

    auto output = captureStdout([&] {
        api.certification(ID, pass);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(NemoAPITester, testPurchasingStockOutput) {
    const string stockCode = "STOCK_CODE";
    const int price = 10;
    const int count = 20;
    const auto golden = format("[NEMO]{} buy stock ( price : {} ) * ( count : {})\n", stockCode, price, count);

    auto output = captureStdout([&] {
        api.purchasingStock(stockCode, price, count);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(NemoAPITester, testSellingStockOutput) {
    const string stockCode = "STOCK_CODE";
    const int price = 10;
    const int count = 20;
    const auto golden = format("[NEMO]{} sell stock ( price : {} ) * ( count : {})\n", stockCode, price, count);

    auto output = captureStdout([&] {
        api.sellingStock(stockCode, price, count);
        });
    EXPECT_EQ(output, golden);
}

TEST_F(NemoAPITester, testGetMarketPriceOutput) {
    const string stockCode = "STOCK_CODE";
    const int minute = 10;

    auto price = api.getMarketPrice(stockCode, minute);
    EXPECT_THAT(price, Ge(5000));
    EXPECT_THAT(price, Le(5900));
}

