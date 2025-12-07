cmake_minimum_required(VERSION 3.12)
project(NetworkApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost - указываем путь напрямую  # не добавляем в master
set(BOOST_ROOT "C:/Program Files/boost-1.88.0")  # не добавляем в master

if(WIN32)  # не добавляем в master
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")  # не добавляем в master
    set(LIBS ws2_32 Mswsock)  # не добавляем в master
endif()  # не добавляем в master

set(SOURCE_DIR src)
set(BSOURCES 
    ${SOURCE_DIR}/chats/core/Client.cpp
    ${SOURCE_DIR}/chats/MessageRepository.cpp
    ${SOURCE_DIR}/chats/ChatRoom.cpp
    ${SOURCE_DIR}/chats/RoomManager.cpp
    ${SOURCE_DIR}/Handlers.cpp
    ${SOURCE_DIR}/Server.cpp
    start_server.cpp
)

add_executable(server ${BSOURCES})
target_include_directories(server PRIVATE "${BOOST_ROOT}") # не добавляем в master
target_link_libraries(server PRIVATE ${LIBS})  # не добавляем в master

# Тесты
if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()