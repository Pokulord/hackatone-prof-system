#pragma once
#include <chrono>

namespace domain::interfaces {

class IAuthConfigProvider {
public:
    virtual ~IAuthConfigProvider() = default;
    virtual int getMaxAttempts() const = 0;
    virtual std::chrono::minutes getLockoutDuration() const = 0;
};

}