cmake_minimum_required(VERSION 3.12)
project(BashMengerClient)

set(CMAKE_CXX_STANDARD 17)

# Boost
if(APPLE)
    include_directories(/opt/homebrew/include)
elseif(WIN32)
    set(BOOST_ROOT "C:/Program Files/boost-1.88.0")
    include_directories("${BOOST_ROOT}")
else()
    find_package(Boost REQUIRED COMPONENTS system)
endif()

add_executable(client
        main.cpp
        Client.h
        Client.cpp
        UI.h
        UI.cpp
)

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32 Mswsock)
else()
    target_link_libraries(client PRIVATE pthread)
endif()

# Тесты
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()