#include "Client.h"

Client::Client() : io(), socket(io), resolver(io) {
    server_ip = "";
    server_port = "";
    room_name = "";
    password = "";
    connected = false;
}

Result Client::setPassword(std::string line) {
    password = line;
    return OK;
}

Result Client::setName(std::string line) {
    name = line;
    return OK;
}

Result Client::setRoomName(std::string line) {
    room_name = line;
    return OK;
}

Result Client::setServerIp(std::string line) {
    server_ip = line;
    return OK;
}

Result Client::setServerPort(std::string line) {
    server_port = line;
    return OK;
}

void Client::send_message(std::string message) {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("msg " + room_name + " " + message + "\n"));
    }
}

void Client::create_room() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd create " + room_name + "\n"));
    }
}

void Client::auth_acc() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd auth " + name + " " + password + "\n"));
    }
}

void Client::reg_acc() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd register " + name + " " + password + "\n"));
    }
}

void Client::del_room() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd delete " + room_name + "\n"));
    }
}

void Client::leave_room() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd leave " + room_name + "\n"));
    }
}

void Client::join_room() {
    if (connected) {
        boost::asio::write(socket, boost::asio::buffer("cmd join " + room_name + "\n"));
    }
}

void Client::connect_to_server(std::vector<std::string> *imported_log, std::vector<std::string> *imported_chat,
                               std::mutex *mut_log) {
    auto endpoints = resolver.resolve("127.0.0.1", "54001"); // IP, порт
    boost::asio::connect(socket, endpoints);
    log = imported_log;
    log_mutex = mut_log;
    chat = imported_chat;
    mutex_log_write("connected", LOG);
    start_receive();
    std::thread([this]() { io.run(); }).detach();
    connected = true;
}

void Client::disconnect() {
    if (connected) {
        socket.close();
        connected = false;
    }
}


void Client::start_receive() {
    boost::asio::async_read_until(socket, buffer, "\n",
                                  [this](boost::system::error_code ec, std::size_t) {
                                      if (!ec) {
                                          std::istream is(&buffer);
                                          std::string line;
                                          std::getline(is, line);
                                          if (!line.empty() && line.back() == '\r')
                                              line.pop_back();

                                          if (line.find(':') != std::string::npos) {
                                              mutex_log_write(line, CHAT);
                                          } else {
                                              mutex_log_write(line, LOG);
                                          }
                                          if(onMessageReceived) {
                                              onMessageReceived();
                                          }
                                          start_receive();
                                      } else {
                                          if (log) {
                                              std::lock_guard<std::mutex> lock(*log_mutex);
                                              log->push_back("server log error: " + ec.message());
                                          }
                                          connected = false;
                                      }
                                  });
}

void Client::mutex_log_write(std::string message, Chat mode) {
    if (log && mode == LOG) {
        std::lock_guard<std::mutex> lock(*log_mutex);
        log->push_back("server log: " + message);
    }
    if (chat && mode == CHAT) {
        std::lock_guard<std::mutex> lock(*log_mutex);
        chat->push_back(message);
    }
}