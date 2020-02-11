cmake_minimum_required(VERSION 3.5)

project(client LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(client client.cpp main.cpp)

target_link_libraries(client Threads::Threads)
