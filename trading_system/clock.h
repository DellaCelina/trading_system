#pragma once

struct ClockInterface {
    virtual ~ClockInterface() = default;

    //현재시각확인기능: now()
    virtual long long now() = 0;
};
