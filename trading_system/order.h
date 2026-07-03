#pragma once

struct OrderInterface {
    virtual void BuyOrder() = 0;
    virtual void SellOrder() = 0;
};
