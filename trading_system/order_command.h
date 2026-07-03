#pragma once

struct IOrderCommand {
    virtual ~IOrderCommand() = default;

    virtual void execute() = 0;
};
