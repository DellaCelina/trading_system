#pragma once

#include <string>

struct LoggerInterface {
    virtual ~LoggerInterface() = default;

    //로그기록기능: log(메시지)
    virtual void log(const std::string& message) = 0;
};
