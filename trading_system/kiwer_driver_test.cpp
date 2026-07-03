#include "gmock/gmock.h"

#include <memory>
#include <functional>

#include "kiwer_api.h"
#include "kiwer_driver.h"

using namespace testing;

struct KiwerDriverTester : public Test {
    void SetUp() override {
        driver.reset(new KiwerDriver(& api));
    }

    std::string captureStdout(std::function<void()> f) {
        internal::CaptureStdout();
        f();
        return internal::GetCapturedStdout();
    }

    std::unique_ptr<KiwerDriver> driver;
    KiwerAPI api;
};

TEST_F(KiwerDriverTester, login2login) {
    auto output = captureStdout([&] {
        driver->login("name", "pass");
        });
    EXPECT_EQ(output, "name login success\n");
}

TEST_F(KiwerDriverTester, buy2buy) {
    auto output = captureStdout([&] {
        driver->buy("code", 1, 2);
        });
    EXPECT_EQ(output, "code : Buy stock ( 1 * 2)\n");
}

TEST_F(KiwerDriverTester, sell2sell) {
    auto output = captureStdout([&] {
        driver->sell("code", 1, 2);
        });
    EXPECT_EQ(output, "code : Sell stock ( 1 * 2)\n");
}

TEST_F(KiwerDriverTester, getPrice2currentPrice) {
    auto price = driver->getPrice("code");
    EXPECT_THAT(price, Ge(5000));
    EXPECT_THAT(price, Le(5900));
}
