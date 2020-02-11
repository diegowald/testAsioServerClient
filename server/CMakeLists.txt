cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server room.cpp session.cpp server.cpp main.cpp)

target_link_libraries(server Threads::Threads)
