cmake_minimum_required(VERSION 3.12)
project(NetworkApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к Boost (Homebrew)
include_directories(/opt/homebrew/include)

# Папка с исходниками
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

# Создаем исполняемый файл
add_executable(server ${BSOURCES})
