#pragma once

#include "core/Client.hpp"
#include "core/UserDataRepository.hpp"
#include "MessageRepository.hpp"

#include <unordered_set>
#include <shared_mutex>
#include <mutex>
#include <vector>

class ChatRoom
{
public:
    ChatRoom(user_id_t admin_uid, std::string name, UserDataRepository& user_data)
        : _admin_uid(admin_uid), room_name(name), _repo(), _user_data(user_data) {}

    void broadcast(const std::string& message, std::shared_ptr<Client> except_part = nullptr);
    void submit_message(const std::string& message, std::shared_ptr<Client> sender);
    void get_history(std::shared_ptr<Client> sender);

    void join_participant(std::shared_ptr<Client> sender);
    void leave_participant(std::shared_ptr<Client> sender);

    bool is_admin(user_id_t sender_id) const;
    bool is_member(user_id_t user_uid) const;

    void add_new_session(std::shared_ptr<Client> sender);
    void remove_session(std::shared_ptr<Client> sender);

    std::string get_nickname(const user_id_t user_id) const;

private:
    mutable std::shared_mutex mtx_;

    user_id_t _admin_uid;
    std::string room_name;

    std::unordered_set<user_id_t> members;
    std::unordered_set<std::shared_ptr<Client>> current_sessions;

    MessageRepository _repo;
    UserDataRepository& _user_data;
};
