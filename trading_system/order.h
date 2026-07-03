#pragma once

struct OrderInterface {
    virtual void execute() = 0;
};

class BuyOrder : public OrderInterface {
    void execute() override {}
};

class SellOrder : public OrderInterface {
    void execute() override {}
};
