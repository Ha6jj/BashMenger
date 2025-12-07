#include "RoomManager.hpp"

void RoomManager::create_room(const std::string& room, USER_ID_T admin_uid)
{
    rooms[room] = std::make_shared<ChatRoom>(admin_uid);
}

void RoomManager::remove_room(const std::string& room)
{
    rooms.erase(room);
}

bool RoomManager::room_exists(const std::string& room) const
{
    return rooms.find(room) != rooms.end();
}

std::shared_ptr<ChatRoom> RoomManager::get_room(const std::string& room)
{
    return rooms[room];
}