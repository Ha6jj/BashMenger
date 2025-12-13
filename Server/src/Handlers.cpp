#include "Handlers.hpp"

void CommandHandler::handle_command(const std::string& subcmd, const std::string& argument, std::shared_ptr<Client> sender)
{
    if (!sender->is_authorized())
    {
        if ((subcmd == "auth" || subcmd == "register"))
        {
            size_t login_end = argument.find(' ');
            if (login_end == std::string::npos)
            {
                sender->deliver_to_client("Format: <command> <login> <password>\n\r");
                return;
            }

            std::string login = argument.substr(0, login_end);
            std::string password = argument.substr(login_end + 1);

            user_id_t result_id = UserDataRepository::invalid_id;
            if (subcmd == "auth")
            {
                result_id = _user_data.authenticate(login, password);
            }
            else if (subcmd == "register")
            {
                result_id = _user_data.register_user(login, password);
            }

            if (result_id == UserDataRepository::invalid_id)
            {
                sender->deliver_to_client("Invalid login or password\n\r");
                return;
            }

            sender->authorize(result_id);

            {
                std::shared_lock lock(subscribtion_mtx);
                auto it = user_subscribtions.find(sender->get_uid());
                if (it != user_subscribtions.end())
                {
                    for (const auto& subscription : it->second)
                    {
                        if (auto room = room_manager.get_room(subscription))
                        {
                            room->add_new_session(sender);
                        }
                    }
                }
            }

            return;
        }
        sender->deliver_to_client("Authorize first\n\r");
        return;
    }

    if (subcmd == "join")
    {
        auto room = room_manager.get_room(argument);
        if (!room)
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }
        
        if (room->is_member(sender->get_uid()))
        {
            sender->deliver_to_client("You are already member of chat: " + argument + "\n\r");
            return;
        }

        {
            std::unique_lock lock(subscribtion_mtx);
            user_subscribtions[sender->get_uid()].insert(argument);
        }

        room->join_participant(sender);
    }
    else if (subcmd == "leave")
    {
        auto room = room_manager.get_room(argument);
        if (!room)
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }
        
        if (!room->is_member(sender->get_uid()))
        {
            sender->deliver_to_client("You aren't member of this chat: " + argument + "\n\r");
            return;
        }

        {
            std::unique_lock lock(subscribtion_mtx);
            user_subscribtions[sender->get_uid()].erase(argument);
        }

        room->leave_participant(sender);
    }
    else if (subcmd == "create")
    {
        if (room_manager.room_exists(argument))
        {
            sender->deliver_to_client("Room called: " + argument + ". Is already exist exist\n\r");
            return;
        }
        room_manager.create_room(argument, sender->get_uid());

        {
            std::unique_lock lock(subscribtion_mtx);
            user_subscribtions[sender->get_uid()].insert(argument);
        }

        room_manager.get_room(argument)->join_participant(sender);
    }
    else if (subcmd == "delete")
    {
        auto room = room_manager.get_room(argument);
        if (!room)
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }

        if (room->is_admin(sender->get_uid()))
        {
            {
                std::unique_lock lock(subscribtion_mtx);
                for (auto& [user_id, subs] : user_subscribtions)
                {
                    subs.erase(argument);
                }
            }

            room_manager.remove_room(argument);
            return;
        }
        sender->deliver_to_client("No permition to delete chat: " + argument + "\n\r");
    }
    else if (subcmd == "history")
    {
        auto room = room_manager.get_room(argument);
        if (!room)
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }

        room->get_history(sender);
    }
    else if (subcmd == "auth" || subcmd == "register")
    {
        sender->deliver_to_client("You are already authorized\n\r");
        return;
    }

}

void CommandHandler::handle_message(const std::string& chat, const std::string& message, std::shared_ptr<Client> sender)
{
    if (!sender->is_authorized()) {
        sender->deliver_to_client("Authorize first\n\r");
        return;
    }

    auto room = room_manager.get_room(chat);
    if (!room)
    {
        sender->deliver_to_client("Room called: " + chat + ". Doesn't exist\n\r");
        return;
    }

    room->submit_message(message, sender);
    
}

void CommandHandler::remove_sessions(std::shared_ptr<Client> sender)
{
    if (!sender->is_authorized()) return;

    std::shared_lock lock(subscribtion_mtx);
    auto it = user_subscribtions.find(sender->get_uid());
    if (it != user_subscribtions.end())
    {
        for (const auto& subscription : it->second)
        {
            if (auto room = room_manager.get_room(subscription))
            {
                room->remove_session(sender);
            }
        }
    }
}
