#include "Handlers.hpp"

void CommandHandler::handle_command(const std::string& subcmd, const std::string& argument, std::shared_ptr<Client> sender)
{
    if (!sender->is_authorized())
    {
        if (subcmd == "auth")
        {
            sender->authorize(argument);
            return;
        }
        sender->deliver_to_client("Authorize first\n\r");
        return;
    }

    if (subcmd == "join")
    {
        if (!room_manager.room_exists(argument))
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }
        room_manager.get_room(argument)->join_participant(sender);
    }
    else if (subcmd == "leave")
    {
        if (!room_manager.room_exists(argument))
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }
        room_manager.get_room(argument)->leave_participant(sender);
    }
    else if (subcmd == "create")
    {
        if (room_manager.room_exists(argument))
        {
            sender->deliver_to_client("Room called: " + argument + ". Is already exist exist\n\r");
            return;
        }
        room_manager.create_room(argument, sender->get_uid());
        room_manager.get_room(argument)->join_participant(sender);
    }
    else if (subcmd == "delete")
    {
        if (!room_manager.room_exists(argument))
        {
            sender->deliver_to_client("Room called: " + argument + ". Doesn't exist\n\r");
            return;
        }

        auto room = room_manager.get_room(argument);
        if (room->is_admin(sender->get_uid()))
        {
            room_manager.remove_room(argument);
            return;
        }
        sender->deliver_to_client("No permition to delete chat: " + argument + "\n\r");
    }
    else if (subcmd == "auth")
    {
        sender->deliver_to_client("You are already authorized\n\r");
        return;
    }

}

void CommandHandler::handle_message(const std::string& chat, const std::string& message, std::shared_ptr<Client> sender)
{
    auto room = room_manager.get_room(chat);
    if (!room_manager.room_exists(chat))
    {
        sender->deliver_to_client("Room called: " + chat + ". Doesn't exist\n\r");
        return;
    }

    if (room->is_member(sender->get_uid()))
    {
        room->broadcast(sender->get_nickname() + ": " + message + '\n', sender);
    }
    else
    {
        sender->deliver_to_client("You aren't member of this chat: " + chat + "\n\r");
    }
}
