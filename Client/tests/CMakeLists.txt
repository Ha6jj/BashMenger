cmake_minimum_required(VERSION 3.14)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(APPLE)
    include_directories(/opt/homebrew/include)
elseif(WIN32)
    set(BOOST_ROOT "C:/Program Files/boost-1.88.0")
    include_directories("${BOOST_ROOT}")
else()
    find_package(Boost REQUIRED COMPONENTS system)
endif()

if(WIN32)
    set(WS2_LIB ws2_32)
    set(MSWSOCK Mswsock)
endif()

# Исходники клиента - ОБНОВЛЁННЫЙ ПУТЬ
set(CLIENT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Client.cpp
)

set(TEST_SOURCES
        main.cpp
        unit/test_client_setters.cpp
        unit/test_client_commands.cpp
        unit/test_client_state.cpp
)

add_executable(client_tests
        ${CLIENT_SOURCES}
        ${TEST_SOURCES}
)

target_include_directories(client_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(client_tests
        GTest::gtest_main
        ${WS2_LIB}
        ${MSWSOCK}
)

if(NOT WIN32)
    target_link_libraries(client_tests pthread)
endif()

enable_testing()
add_test(NAME ClientTests COMMAND client_tests)