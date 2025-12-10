cmake_minimum_required(VERSION 3.28)
project(BashMangerClient)

set(CMAKE_CXX_STANDARD 17)

add_executable(BashMangerClient main.cpp
        Client.h
        Client.cpp
        UI.h
        UI.cpp)

include_directories(/opt/homebrew/include)