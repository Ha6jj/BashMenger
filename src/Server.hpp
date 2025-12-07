#pragma once

#include "Handlers.hpp"

#include <optional>

class Server
{
public:
    Server(io::io_context& io_context, std::uint16_t port, RoomManager& room_manager);

    void async_accept();

private:
    void client_handler(const std::string& input, std::shared_ptr<Client> sender);
    std::string trim(const std::string& str);

    io::io_context& _io_context;
    tcp::acceptor _acceptor;
    std::optional<tcp::socket> _socket;
    std::unordered_set<std::shared_ptr<Client>> clients;
    CommandHandler command_handler;
};
