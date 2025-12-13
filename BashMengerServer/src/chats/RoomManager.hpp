#pragma once

#include "ChatRoom.hpp"

#include <unordered_map>

class RoomManager
{
public:
    RoomManager(UserDataRepository& user_data) : _user_data(user_data) {}

    void create_room(const std::string& room, user_id_t admin_uid);
    void remove_room(const std::string& room);

    bool room_exists(const std::string& room) const;
    std::shared_ptr<ChatRoom> get_room(const std::string& room) const;
    
private:
    mutable std::shared_mutex mtx;

    std::unordered_map<std::string, std::shared_ptr<ChatRoom>> rooms;
    UserDataRepository& _user_data;
};
