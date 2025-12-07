#include "ChatRoom.hpp"

#include <sstream>
#include <iomanip>

void ChatRoom::broadcast(const std::string& message, std::shared_ptr<Client> except_part)
{
    for (auto& participant : participants)
    {
        if (participant != except_part)
        {
            participant->deliver_to_client(message);
        }
    }
}

void ChatRoom::submit_message(std::shared_ptr<Client> sender, const std::string& message)
{
    _repo.add_message(sender->get_nickname(), message);
    std::string formatted_msg = sender->get_nickname() + ": " + message + "\n\r";
    broadcast(formatted_msg);
}

void ChatRoom::join_participant(std::shared_ptr<Client> participant)
{
    if (!is_member(participant->get_uid()))
    {
        participants.insert(participant);
        broadcast("Participant joined: " + participant->get_nickname() + "\n\r", participant);

        auto missed_messages = _repo.get_missed_messages(participant->get_nickname());
        if (!missed_messages.empty()) {
            participant->deliver_to_client("--- History ---\n\r");
            for (const auto& msg : missed_messages)
            {
                auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
                std::stringstream ss;
                ss << "[" << std::put_time(std::localtime(&time_t), "%H:%M") << "] " 
                   << msg.sender << ": " << msg.content << "\n\r";
                
                participant->deliver_to_client(ss.str());
            }
            participant->deliver_to_client("----------------\n\r");
        }

        _repo.mark_as_read(participant->get_nickname(), _repo.get_latest_message_id());
    }
    else
    {
        participant->deliver_to_client("You are already member of chat: " + room_name + "\n\r");
    }
}

void ChatRoom::leave_participant(std::shared_ptr<Client> participant)
{
    if (is_member(participant->get_uid()))
    {
        _repo.mark_as_read(participant->get_nickname(), _repo.get_latest_message_id());

        participants.erase(participant);
        broadcast("Participant left: " + participant->get_nickname() + "\n\r", participant);
    }
    else
    {
        participant->deliver_to_client("You aren't member of this chat: " + room_name + "\n\r");
    }
}

bool ChatRoom::is_admin(USER_ID_T participant_id) const
{
    return _admin_uid == participant_id;
}

bool ChatRoom::is_member(USER_ID_T user_uid) const
{
    for (auto& chat_participant : participants)
    {
        if (user_uid == chat_participant->get_uid())
        {
            return true;
        }
    }
    return false;
}
