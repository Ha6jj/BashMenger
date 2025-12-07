#pragma once

#include "chats/RoomManager.hpp"

class CommandHandler
{
public:
    CommandHandler(RoomManager& room_manager) : room_manager(room_manager) {}
    
    void handle_command(const std::string& subcmd, const std::string& argument, std::shared_ptr<Client> sender);
    void handle_message(const std::string& chat, const std::string& message, std::shared_ptr<Client> sender);
    
private:
    RoomManager& room_manager;
};
