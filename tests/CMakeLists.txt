cmake_minimum_required(VERSION 3.14)

# Отключаем ненужные таргеты Google Test
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Google Test - автозагрузка
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Boost
set(BOOST_ROOT "C:/Program Files/boost-1.88.0")

if(WIN32)
    set(WS2_LIB ws2_32)
    set(MSWSOCK Mswsock)
endif()

# Исходники проекта
set(PROJECT_SOURCES
        ../src/chats/core/Client.cpp
        ../src/chats/MessageRepository.cpp
        ../src/chats/ChatRoom.cpp
        ../src/chats/RoomManager.cpp
        ../src/Handlers.cpp
        ../src/Server.cpp
)

# Тесты (пока только эти)
set(TEST_SOURCES
        main.cpp
        test_helpers.cpp
        unit/test_room_manager.cpp
        unit/test_client.cpp
)

add_executable(tests
        ${PROJECT_SOURCES}
        ${TEST_SOURCES}
)

target_include_directories(tests PRIVATE
        "${BOOST_ROOT}"
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tests
        GTest::gtest_main
        ${WS2_LIB}
        ${MSWSOCK}
)

target_compile_definitions(tests PRIVATE
        _WIN32_WINNT=0x0601
)

enable_testing()
add_test(NAME AllTests COMMAND tests)