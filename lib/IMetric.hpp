#pragma once
#include <string>

// Interface for all metric types
class IMetric {
public:
    virtual ~IMetric() = default;

    // Returns the metric name
    virtual std::string getName() const = 0;

    // Returns the current value as a formatted string
    virtual std::string getValueString() const = 0;

    // Resets the internal state after logging
    virtual void reset() = 0;

    // Returns value string and resets the metric
    virtual std::string getValueAndReset() {
        auto value = getValueString();
        reset();
        return value;
    }
};