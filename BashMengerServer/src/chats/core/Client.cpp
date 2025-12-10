#include "Client.hpp"

void Session::start(message_handler_impl&& on_mesage, error_handler&& on_error)
{
    _on_message = std::move(on_mesage);
    _on_error = std::move(on_error);
    async_read();
}

void Session::deliver_to_client(const std::string& message)
{
    bool write_in_progress = !outgoing.empty();
    outgoing.push(message);
    if (!write_in_progress)
    {
        async_write();
    }
}

void Session::async_read()
{
    io::async_read_until(_socket, buffer, '\n',
        std::bind(&Session::on_read, shared_from_this(), _1, _2)
    );
}

void Session::on_read(error_code error, std::size_t bytes)
{
    if (!error)
    {
        std::istream is(&buffer);
        std::string line;
        if (std::getline(is, line)) 
        {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            _on_message(line, std::dynamic_pointer_cast<Client>(shared_from_this()));
        }

        async_read();
    }
    else
    {
        _socket.close();
        if(_on_error) _on_error();
    }
}

void Session::async_write()
{
    io::async_write(_socket, io::buffer(outgoing.front()),
        std::bind(&Session::on_write, shared_from_this(), _1, _2)
    );
}

void Session::on_write(error_code error, std::size_t bytes)
{
    if (!error)
    {
        outgoing.pop();
        if (!outgoing.empty())
        {
            async_write();
        }
    }
    else
    {
        _socket.close(error);
        _on_error();
    }
}

// Client

void Client::authorize(const user_id_t id)
{
    std::lock_guard lock(auth_mutex);
    authorized = true;
    uid = id;
}

bool Client::is_authorized() const
{
    std::lock_guard lock(auth_mutex);
    return authorized;
}

user_id_t Client::get_uid() const
{
    std::lock_guard lock(auth_mutex);
    return uid;
}
