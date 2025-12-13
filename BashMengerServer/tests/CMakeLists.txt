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
if(APPLE)
    include_directories(/opt/homebrew/include)
elseif(WIN32)
    set(BOOST_ROOT "C:/Program Files/boost-1.88.0")
    include_directories("${BOOST_ROOT}")
else()
    find_package(Boost 1.74.0 REQUIRED COMPONENTS system)
endif()

if(WIN32)
    set(WS2_LIB ws2_32)
    set(MSWSOCK Mswsock)
endif()

# Исходники проекта
set(PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chats/core/Client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chats/core/UserDataRepository.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chats/MessageRepository.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chats/ChatRoom.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/chats/RoomManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Handlers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/Server.cpp
)

# Тесты
set(TEST_SOURCES
    main.cpp
    test_helpers.cpp
    unit/test_user_data_repository.cpp
    unit/test_client.cpp
    unit/test_room_manager.cpp
    unit/test_chat_room.cpp
)

add_executable(server_tests
    ${PROJECT_SOURCES}
    ${TEST_SOURCES}
)

target_include_directories(server_tests PRIVATE
    "${BOOST_ROOT}"
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(server_tests
    GTest::gtest_main 
    ${WS2_LIB} 
    ${MSWSOCK}
)

if(NOT WIN32)
    target_link_libraries(server_tests pthread)
endif()

target_compile_definitions(server_tests PRIVATE
    _WIN32_WINNT=0x0601
)

enable_testing()
add_test(NAME AllTests COMMAND server_tests)
