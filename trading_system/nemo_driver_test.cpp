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
    EXPECT_EQ(output, "[NEMO]code sell stock ( price : 1 ) * (count : 2)\n");
}

TEST_F(NemoDriverTester, getPrice2getMarketPrice) {
    auto price = driver->getPrice("code");
    EXPECT_THAT(price, Ge(5000));
}
