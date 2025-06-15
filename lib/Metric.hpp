#pragma once
#include "IMetric.hpp"
#include <mutex>
#include <atomic>
#include <sstream>
#include <type_traits>

template<typename T>
class Metric : public IMetric {
public:
    enum class Aggregation { Sum, Avg, Max, Min };

    Metric(const std::string& name, Aggregation agg = Aggregation::Sum)
        : name_(name), value_(0), aggregation_(agg), count_(0) {
    }

    void add(T v) {
        std::lock_guard<std::mutex> lock(mtx_);
        value_ += v;
        count_++;
    }

    std::string getName() const override {
        return name_;
    }

    std::string getValueString() const override {
        T valueCopy;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            valueCopy = getValueLocked();
        }
        std::ostringstream oss;
        oss << "\"" << name_ << "\" " << valueCopy;
        return oss.str();
    }

    void reset() override {
        std::lock_guard<std::mutex> lock(mtx_);
        value_ = 0;
        count_ = 0;
    }

    std::string getValueAndReset() override {
        T valueCopy;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            valueCopy = getValueLocked();
            value_ = 0;
            count_ = 0;
        }
        std::ostringstream oss;
        oss << "\"" << name_ << "\" " << valueCopy;
        return oss.str();
    }

    void setAggregation(Aggregation agg) {
        std::lock_guard<std::mutex> lock(mtx_);
        aggregation_ = agg;
    }

private:
    T getValueLocked() const {
        switch (aggregation_) {
        case Aggregation::Avg:
            if constexpr (std::is_floating_point_v<T>) {
                return count_ > 0 ? static_cast<T>(value_) / count_ : 0;
            }
            else {
                return count_ > 0 ? value_ / static_cast<T>(count_) : 0;
            }
        default:
            return value_;
        }
    }

    std::string name_;
    T value_;
    Aggregation aggregation_;
    mutable std::mutex mtx_;
    size_t count_;
};
