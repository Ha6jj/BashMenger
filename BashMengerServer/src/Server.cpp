#include "Server.hpp"
#include <iostream>
Server::Server(io::io_context& io_context, std::uint16_t port)
    : _io_context(io_context), _acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {}

void Server::async_accept()
{
    _socket.emplace(_io_context);

    _acceptor.async_accept(*_socket, 
        [&](error_code error)
        {
            auto client = std::make_shared<Client>(std::move(*_socket));
            clients.insert(client);

            client->start(
                std::bind(&Server::client_handler, this, _1, _2),
                [&, weak = std::weak_ptr(client)]
                {
                    if (auto shared = weak.lock())
                    {
                        command_handler.remove_sessions(shared);
                        clients.erase(shared);
                    }
                }

            );

            async_accept();
        }
    );
}

void Server::client_handler(const std::string& input, std::shared_ptr<Client> sender)
{
    std::cout << input << '\n';
    if (input.empty())
    {
        sender->deliver_to_client("Empty package\n\r");
        return;
    }
    
    size_t second_item_start = 4;
    size_t second_item_end = input.find(' ', second_item_start);
    if (second_item_end == std::string::npos)
    {
        sender->deliver_to_client("Unable to parse command\n\r");
        return;
    }

    std::string first_item = input.substr(second_item_start, second_item_end - second_item_start);
    std::string other = trim(input.substr(second_item_end + 1));
    if (input.substr(0, 3) == "cmd")
    {
        command_handler.handle_command(first_item, other, sender);
    }
    else if (input.substr(0, 3) == "msg")
    {
        command_handler.handle_message(first_item, other, sender);
    }
    else
    {
        sender->deliver_to_client("Wrong format\n\r");
    }

}

std::string Server::trim(const std::string& str)
{
    const std::string whitespace = " \t\n\r";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}
