#include "RoomManager.hpp"

void RoomManager::create_room(const std::string& room, user_id_t admin_uid)
{
    std::unique_lock lock(mtx);

    if (rooms.find(room) != rooms.end())
    {
        throw std::runtime_error("Room already exists: " + room);
    }
    rooms[room] = std::make_shared<ChatRoom>(admin_uid, room, _user_data);
}

void RoomManager::remove_room(const std::string& room)
{
    std::unique_lock lock(mtx);
    rooms.erase(room);
}

bool RoomManager::room_exists(const std::string& room) const
{
    std::shared_lock lock(mtx);
    return rooms.find(room) != rooms.end();
}

std::shared_ptr<ChatRoom> RoomManager::get_room(const std::string& room) const
{
    std::shared_lock lock(mtx);

    auto it = rooms.find(room);
    if (it != rooms.end())
    {
        return it->second;
    }

    return nullptr;
}
