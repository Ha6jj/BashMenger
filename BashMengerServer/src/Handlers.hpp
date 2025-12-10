#pragma once

#include "chats/RoomManager.hpp"

class CommandHandler
{
public:
    CommandHandler()
        : _user_data(), room_manager(_user_data) {}

    void handle_command(const std::string& subcmd, const std::string& argument, std::shared_ptr<Client> sender);
    void handle_message(const std::string& chat, const std::string& message, std::shared_ptr<Client> sender);

    void remove_sessions(std::shared_ptr<Client> sender);
    
private:
    mutable std::shared_mutex subscribtion_mtx;
    std::unordered_map<user_id_t, std::unordered_set<std::string>> user_subscribtions;

    UserDataRepository _user_data;
    RoomManager room_manager;
};
