#pragma once

#include <boost/asio.hpp>

#include "Types.hpp"

#include <shared_mutex>
#include <queue>
#include <string>

namespace io = boost::asio;
using tcp = io::ip::tcp;
using error_code = boost::system::error_code;
using namespace std::placeholders;

class Client;
using message_handler_impl = std::function<void(std::string, std::shared_ptr<Client>)>;
using error_handler = std::function<void()>;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket&& socket) : _socket(std::move(socket)) {};
    virtual ~Session() = default; 

    void start(message_handler_impl&& on_mesage, error_handler&& on_error);
    void deliver_to_client(const std::string& message);

private:
    void async_read();
    void on_read(error_code error, std::size_t bytes);

    void async_write();
    void on_write(error_code error, std::size_t bytes);
    
    tcp::socket _socket;
    io::streambuf buffer;
    std::queue<std::string> outgoing;
    message_handler_impl _on_message;
    error_handler _on_error;
};

class Client : public Session
{
public:
    Client(tcp::socket&& socket) : Session(std::move(socket)) {}

    void authorize(const user_id_t id);
    bool is_authorized() const;
    user_id_t get_uid() const;

private:
    mutable std::shared_mutex auth_mutex;

    bool authorized = false;
    user_id_t uid;
};
