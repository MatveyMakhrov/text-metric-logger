cmake_minimum_required(VERSION 3.10)
project(text-metric-logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(metrics_app lib/main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(metrics_app PRIVATE Threads::Threads)