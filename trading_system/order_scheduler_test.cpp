#include "gmock/gmock.h"

#include <memory>

#include "order_command.h"
#include "clock.h"
#include "logger.h"
#include "order_scheduler.h"

using namespace testing;

struct MockClock : public ClockInterface {
    MOCK_METHOD(long long, now, (), (override));
};

struct MockLogger : public LoggerInterface {
    MOCK_METHOD(void, log, (const std::string& message), (override));
};

struct MockOrderCommand : public IOrderCommand {
    MOCK_METHOD(void, execute, (), (override));
};

struct OrderSchedulerTester : public Test {
    void SetUp() override {
        scheduler.setClock(&clock);
        scheduler.setLogger(&logger);
    }

    StrictMock<MockClock> clock;
    NiceMock<MockLogger> logger;
    OrderScheduler scheduler;
};

TEST_F(OrderSchedulerTester, startScheduledOrderExecutesOrderWhenTimeReached) {
    auto order = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(order, 1000);

    EXPECT_CALL(clock, now()).WillOnce(Return(1000));
    EXPECT_CALL(*order, execute()).Times(1);
    EXPECT_CALL(logger, log(_)).Times(2);

    scheduler.startScheduledOrder();
}

TEST_F(OrderSchedulerTester, startScheduledOrderDoesNotExecuteOrderBeforeTime) {
    auto order = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(order, 1000);

    EXPECT_CALL(clock, now()).WillOnce(Return(999));
    EXPECT_CALL(*order, execute()).Times(0);

    scheduler.startScheduledOrder();
}

TEST_F(OrderSchedulerTester, startScheduledOrderDoesNotExecuteSameOrderTwice) {
    auto order = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(order, 1000);

    EXPECT_CALL(clock, now()).WillOnce(Return(1000));
    EXPECT_CALL(*order, execute()).Times(1);
    scheduler.startScheduledOrder();

    EXPECT_CALL(clock, now()).WillOnce(Return(2000));
    scheduler.startScheduledOrder();
}

TEST_F(OrderSchedulerTester, startScheduledOrderExecutesOnlyDueOrders) {
    auto dueOrder = std::make_shared<StrictMock<MockOrderCommand>>();
    auto futureOrder = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(dueOrder, 1000);
    scheduler.scheduleOrder(futureOrder, 5000);

    EXPECT_CALL(clock, now()).WillOnce(Return(1000));
    EXPECT_CALL(*dueOrder, execute()).Times(1);
    EXPECT_CALL(*futureOrder, execute()).Times(0);

    scheduler.startScheduledOrder();
}

TEST_F(OrderSchedulerTester, flushScheduledOrderExecutesEveryOrderRegardlessOfTime) {
    auto nearOrder = std::make_shared<StrictMock<MockOrderCommand>>();
    auto farOrder = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(nearOrder, 1000);
    scheduler.scheduleOrder(farOrder, 999999);

    EXPECT_CALL(*nearOrder, execute()).Times(1);
    EXPECT_CALL(*farOrder, execute()).Times(1);
    EXPECT_CALL(logger, log(_)).Times(4);

    scheduler.flushScheduledOrder();
}

TEST_F(OrderSchedulerTester, flushScheduledOrderClearsQueue) {
    auto order = std::make_shared<StrictMock<MockOrderCommand>>();
    scheduler.scheduleOrder(order, 1000);

    EXPECT_CALL(*order, execute()).Times(1);
    scheduler.flushScheduledOrder();

    scheduler.flushScheduledOrder();
}
