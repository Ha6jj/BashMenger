#include "ChatRoom.hpp"

#include <sstream>
#include <iomanip>
#include <vector>

void ChatRoom::broadcast(const std::string& message, std::shared_ptr<Client> except_part)
{
    std::vector<std::shared_ptr<Client>> sessions_snapshot;

    {
        std::shared_lock lock(mtx_);
        sessions_snapshot.reserve(current_sessions.size());
        sessions_snapshot.insert(sessions_snapshot.end(), current_sessions.begin(), current_sessions.end());
    }

    for (auto& participant : sessions_snapshot)
    {
        if (participant != except_part)
        {
            participant->deliver_to_client(message);
        }
    }
}

void ChatRoom::submit_message(const std::string& message, std::shared_ptr<Client> sender)
{
    if (!is_member(sender->get_uid()))
    {
        sender->deliver_to_client("You aren't member of this chat: " + room_name + "\n\r");
        return;
    }

    _repo.add_message(sender->get_uid(), message);

    std::string formatted_msg = get_nickname(sender->get_uid()) + ": " + message + "\n\r";
    broadcast(formatted_msg, sender);

    std::vector<std::shared_ptr<Client>> sessions_snapshot;
    {
        std::shared_lock lock(mtx_);
        sessions_snapshot.insert(sessions_snapshot.end(), current_sessions.begin(), current_sessions.end());
    }

    for (auto& participant : sessions_snapshot)
    {
        _repo.mark_as_read(participant->get_uid(), _repo.get_latest_message_id());
    }
}

void ChatRoom::get_history(std::shared_ptr<Client> sender)
{
    if (!is_member(sender->get_uid()))
    {
        sender->deliver_to_client("You aren't member of this chat: " + room_name + "\n\r");
        return;
    }

    auto missed_messages = _repo.get_missed_messages(sender->get_uid());
    if (!missed_messages.empty()) {
        sender->deliver_to_client("--- History ---\n\r");
        for (const auto& msg : missed_messages)
        {
            auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
            std::stringstream ss;
            ss << "[" << std::put_time(std::localtime(&time_t), "%H:%M") << "] " 
               << get_nickname(msg.sender_id) << ": " << msg.content << "\n\r";
                
            sender->deliver_to_client(ss.str());
        }
        sender->deliver_to_client("----------------\n\r");
    }

    _repo.mark_as_read(sender->get_uid(), _repo.get_latest_message_id());
}

void ChatRoom::join_participant(std::shared_ptr<Client> sender)
{
    user_id_t uid = sender->get_uid();
    {
        std::unique_lock lock(mtx_);
        members.insert(uid);   
    }

    add_new_session(sender);
    broadcast("Participant joined: " + get_nickname(uid) + "\n\r", sender);
}

void ChatRoom::leave_participant(std::shared_ptr<Client> sender)
{   
    user_id_t uid = sender->get_uid();
    {
        std::unique_lock lock(mtx_);
        members.erase(uid);
    }

    remove_session(sender);
    broadcast("Participant left: " + get_nickname(uid) + "\n\r", sender);
}

void ChatRoom::add_new_session(std::shared_ptr<Client> sender)
{
    std::unique_lock lock(mtx_);
    current_sessions.insert(sender);
}

void ChatRoom::remove_session(std::shared_ptr<Client> sender)
{
    std::unique_lock lock(mtx_);
    current_sessions.erase(sender);
}

bool ChatRoom::is_admin(user_id_t sender_id) const
{
    std::shared_lock lock(mtx_); 
    return _admin_uid == sender_id;
}

bool ChatRoom::is_member(user_id_t user_uid) const
{
    std::shared_lock lock(mtx_);
    return members.find(user_uid) != members.end();
}

std::string ChatRoom::get_nickname(const user_id_t user_id) const
{
    auto nickname = _user_data.get_nickname(user_id);
    return nickname.value_or("Undefined user");
}
