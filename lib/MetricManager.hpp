#pragma once
#include "IMetric.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <mutex>

class MetricManager {
public:
    MetricManager(const std::string& filename, int intervalMs = 1000)
        : outputFilename(filename), interval(intervalMs), running(true) {
        if (!openFile()) {
            throw std::runtime_error("Cannot open metrics file: " + filename);
        }
        writerThread = std::thread(&MetricManager::writerLoop, this);
    }

    ~MetricManager() {
        running = false;
        if (writerThread.joinable()) {
            writerThread.join();
        }
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }

    void registerMetric(std::shared_ptr<IMetric> metric) {
        std::lock_guard<std::mutex> lock(mtx);
        metrics.push_back(metric);
    }

private:
    std::vector<std::shared_ptr<IMetric>> metrics;
    std::mutex mtx;
    std::string outputFilename;  // имя файла
    std::ofstream fileStream;    // поток для записи
    std::mutex fileMtx;
    int interval;
    std::atomic<bool> running;
    std::thread writerThread;

    bool openFile() {
        std::lock_guard<std::mutex> lock(fileMtx);
        fileStream.open(outputFilename, std::ios::app);
        return fileStream.is_open();
    }

    void writerLoop() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));

            std::ostringstream line;
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            std::tm tm_now;
            localtime_s(&tm_now, &time);

            line << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S")
                << "." << std::setfill('0') << std::setw(3) << ms.count();

            {
                std::lock_guard<std::mutex> lock(mtx);
                for (const auto& metric : metrics) {
                    line << " " << metric->getValueAndReset();
                }
            }

            {
                std::lock_guard<std::mutex> lock(fileMtx);
                if (fileStream.is_open()) {
                    fileStream << line.str() << std::endl;
                    fileStream.flush();
                }
            }
        }
    }
};