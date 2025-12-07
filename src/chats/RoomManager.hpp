#pragma once

#include "ChatRoom.hpp"

#include <unordered_map>

class RoomManager
{
public:
    void create_room(const std::string& room, USER_ID_T admin_uid);
    void remove_room(const std::string& room);

    bool room_exists(const std::string& room) const;
    std::shared_ptr<ChatRoom> get_room(const std::string& room);
    
private:
    std::unordered_map<std::string, std::shared_ptr<ChatRoom>> rooms;
};