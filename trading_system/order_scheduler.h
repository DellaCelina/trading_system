#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "order_command.h"
#include "clock.h"
#include "logger.h"

//AutoTradingSystem이 예약 주문 기능을 위임할 인터페이스: DI로 연결하여 Mock 검증이 가능하다.
struct OrderSchedulerInterface {
    virtual ~OrderSchedulerInterface() = default;

    //예약기능 : scheduleOrder(주문, 실행시각) - 주문을 큐에 저장한다.
    virtual void scheduleOrder(std::shared_ptr<IOrderCommand> order, long long executeAt) = 0;

    //실행시각이 도달한 예약 주문만 실행한다.
    virtual void startScheduledOrder() = 0;

    //실행시각과 무관하게 큐에 남은 모든 예약 주문을 즉시 실행한다.
    virtual void flushScheduledOrder() = 0;
};

class OrderScheduler : public OrderSchedulerInterface {
public:
    //Dependency Injection for clock
    void setClock(ClockInterface* clock) {
        this->clock = clock;
    }

    //Dependency Injection for logger
    void setLogger(LoggerInterface* logger) {
        this->logger = logger;
    }

    void scheduleOrder(std::shared_ptr<IOrderCommand> order, long long executeAt) override {
        scheduledOrders.push_back({ order, executeAt });
    }

    //시간이 도달하면 자동으로 실행된다.
    void startScheduledOrder() override {
        long long currentTime = clock->now();
        executeIf([currentTime](long long executeAt) { return executeAt <= currentTime; });
    }

    //남은 예약 주문을 시각과 무관하게 모두 실행한다.
    void flushScheduledOrder() override {
        executeIf([](long long) { return true; });
    }

private:
    struct ScheduledOrder {
        std::shared_ptr<IOrderCommand> order;
        long long executeAt;
    };

    //모든실행은자동으로로그를남긴다.
    void executeIf(const std::function<bool(long long)>& shouldExecute) {
        for (auto it = scheduledOrders.begin(); it != scheduledOrders.end();) {
            if (shouldExecute(it->executeAt)) {
                logger->log("Order execution started");
                it->order->execute();
                logger->log("Order execution finished");
                it = scheduledOrders.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<ScheduledOrder> scheduledOrders;
    ClockInterface* clock = nullptr;
    LoggerInterface* logger = nullptr;
};
