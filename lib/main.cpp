#include "Metric.hpp"
#include "MetricManager.hpp"
#include <thread>
#include <chrono>
#include <random>
#include <atomic>

int main() {
    MetricManager manager("metrics_output.txt");

    // Создание метрик
    auto cpuUtil = std::make_shared<Metric<double>>("CPU", Metric<double>::Aggregation::Avg);
    auto httpReqs = std::make_shared<Metric<int>>("HTTP requests RPS");
    auto temp = std::make_shared<Metric<float>>("Temperature", Metric<float>::Aggregation::Avg);
    auto errors = std::make_shared<Metric<int>>("Errors");
    auto memory = std::make_shared<Metric<double>>("Memory Usage", Metric<double>::Aggregation::Avg);
    auto diskIO = std::make_shared<Metric<int>>("Disk IO");

    // Регистрация метрик
    manager.registerMetric(cpuUtil);
    manager.registerMetric(httpReqs);
    manager.registerMetric(temp);
    manager.registerMetric(errors);
    manager.registerMetric(memory);
    manager.registerMetric(diskIO);

    std::atomic<bool> running{ true };

    // Поток, имитирующий генерацию событий
    std::thread simulator([&]() {
        std::default_random_engine gen;
        std::uniform_real_distribution<double> cpuDist(0.0, 4.0);           // до 4 ядер
        std::uniform_int_distribution<int> reqDist(0, 100);
        std::uniform_real_distribution<float> tempDist(35.0f, 60.0f);       // градусы
        std::uniform_int_distribution<int> errDist(0, 5);
        std::uniform_real_distribution<double> memDist(0.0, 16.0);          // GB
        std::uniform_int_distribution<int> diskDist(0, 500);                // MB/s

        while (running) {
            cpuUtil->add(cpuDist(gen));
            httpReqs->add(reqDist(gen));
            temp->add(tempDist(gen));
            errors->add(errDist(gen));
            memory->add(memDist(gen));
            diskIO->add(diskDist(gen));
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    running = false;
    simulator.join();
    return 0;
}
