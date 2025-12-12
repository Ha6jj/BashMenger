cmake_minimum_required(VERSION 3.12)
project(NetworkApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(Boost 1.74.0 REQUIRED COMPONENTS system)
set(BOOST_ROOT "C:/Program Files/boost-1.88.0")

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
    set(LIBS ws2_32 Mswsock)
endif()

set(SOURCE_DIR src)
set(BSOURCES 
    ${SOURCE_DIR}/chats/core/Client.cpp
    ${SOURCE_DIR}/chats/core/UserDataRepository.cpp
    ${SOURCE_DIR}/chats/MessageRepository.cpp
    ${SOURCE_DIR}/chats/ChatRoom.cpp
    ${SOURCE_DIR}/chats/RoomManager.cpp
    ${SOURCE_DIR}/Handlers.cpp
    ${SOURCE_DIR}/Server.cpp
    start_server.cpp
)

add_executable(server ${BSOURCES})

#target_link_libraries(server PRIVATE Boost::system)
#target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(server PRIVATE "${BOOST_ROOT}")
target_link_libraries(server PRIVATE ${LIBS})
if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
    add_subdirectory(tests)
endif()