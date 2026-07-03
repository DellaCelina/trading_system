#include "gmock/gmock.h"

#include <memory>
#include <functional>

#include "nemo_api.h"
#include "nemo_driver.h"

using namespace testing;

struct NemoDriverTester : public Test {
    void SetUp() override {
        driver.reset(new NemoDriver(& api));
    }

    std::string captureStdout(std::function<void()> f) {
        internal::CaptureStdout();
        f();
        return internal::GetCapturedStdout();
    }

    std::unique_ptr<NemoDriver> driver;
    NemoAPI api;

    static constexpr int PRICE_MULT_MAX = 9;
    static constexpr int PRICE_MULT_RATIO = 100;

    static constexpr int PRICE_MIN = 5000;
    static constexpr int PRICE_MAX = PRICE_MIN + PRICE_MULT_MAX * PRICE_MULT_RATIO;
};

TEST_F(NemoDriverTester, login2Certificate) {
    auto output = captureStdout([&] {
        driver->login("name", "pass");
        });
    EXPECT_EQ(output, "[NEMO]name login GOOD\n");
}

TEST_F(NemoDriverTester, buy2PurchasingStock) {
    auto output = captureStdout([&] {
        driver->buy("code", 1, 2);
        });
    EXPECT_EQ(output, "[NEMO]code buy stock ( price : 1 ) * ( count : 2)\n");
}

TEST_F(NemoDriverTester, sell2SellingStock) {
    auto output = captureStdout([&] {
        driver->sell("code", 1, 2);
        });
    EXPECT_EQ(output, "[NEMO]code sell stock ( price : 1 ) * ( count : 2)\n");
}

TEST_F(NemoDriverTester, getPrice2getMarketPrice) {
    auto price = driver->getPrice("code");
    EXPECT_THAT(price, Ge(PRICE_MIN));
    EXPECT_THAT(price, Le(PRICE_MAX));
}
