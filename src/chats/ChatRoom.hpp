#pragma once

#include "core/Client.hpp"
#include "MessageRepository.hpp"

#include <unordered_set>

class ChatRoom
{
public:
    ChatRoom(USER_ID_T admin_uid) : _admin_uid(admin_uid) {}

    void broadcast(const std::string& message, std::shared_ptr<Client> except_part = nullptr);

    void submit_message(std::shared_ptr<Client> sender, const std::string& message);

    void join_participant(std::shared_ptr<Client> participant);
    void leave_participant(std::shared_ptr<Client> participant);

    bool is_admin(USER_ID_T participant_id) const;
    bool is_member(USER_ID_T user_uid) const;

private:
    USER_ID_T _admin_uid;
    std::string room_name;
    std::unordered_set<std::shared_ptr<Client>> participants;
    MessageRepository _repo;
};
