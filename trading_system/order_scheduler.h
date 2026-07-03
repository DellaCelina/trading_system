#pragma once

#include <memory>

#include "order.h"
#include "timer.h"

//OrderScheduler 가 예약 큐를 관리
//특정시간혹은명령어시준비된명령들을자동실행
//실행전후로자동로깅

struct OrderSchedulerInterface {
    virtual void add(time_t time, OrderInterface* order) = 0;

    // Queue의 첫번째 실행, 다음 order까지의 시간 return
    // 더이상 order가 없으면 return -1
    virtual time_t execute() = 0;
};

class OrderScheduler : public OrderSchedulerInterface {
public:
    void add(time_t time, OrderInterface* order) override {}
    time_t execute() override {}
};
