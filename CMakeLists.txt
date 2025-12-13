cmake_minimum_required(VERSION 3.12)
project(BashMenger)

set(CMAKE_CXX_STANDARD 17)

# Подпроекты
add_subdirectory(BashMengerServer)
add_subdirectory(BashMengerClient)