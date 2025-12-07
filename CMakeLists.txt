cmake_minimum_required(VERSION 3.12)
project(NetworkApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.74.0 REQUIRED COMPONENTS system)

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

target_link_libraries(server PRIVATE Boost::system)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
