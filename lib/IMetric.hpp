#pragma once
#include <string>

class IMetric {
public:
    virtual ~IMetric() = default;
    virtual std::string getName() const = 0;
    virtual std::string getValueString() const = 0;
    virtual void reset() = 0;

    virtual std::string getValueAndReset() {
        auto value = getValueString();
        reset();
        return value;
    }
};