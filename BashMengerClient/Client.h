//
// Created by Feliks Rogachev on 08.12.2025.
//
#ifndef BASHMANGERCLIENT_CLIENT_H
#define BASHMANGERCLIENT_CLIENT_H

#include <string>
#include <boost/asio.hpp>
#include <vector>
#include <iostream>
#include <mutex>

enum Result {
    OK, WRONG, EXCEPTION
};

enum Chat {
    LOG, CHAT
};
using boost::asio::ip::tcp;

class Client {
public:
    Client();

    void send_message(std::string message);

    void join_room();

    void create_room();

    void reg_acc();

    void auth_acc();

    void del_room();

    void leave_room();

    void connect_to_server(std::vector<std::string> *imported_log, std::vector<std::string> *imported_chat,
                           std::mutex *mut_log);

    void start_receive();

    void disconnect();

    void mutex_log_write(std::string message, Chat mode);

    std::function<void()> onMessageReceived;

    Result setServerIp(std::string line);

    Result setServerPort(std::string line);

    Result setRoomName(std::string line);

    Result setName(std::string line);

    Result setPassword(std::string line);

    std::string get_history();

    // Методы для тестирования
    std::string format_auth_command() const;

    std::string format_register_command() const;

    std::string format_join_room_command() const;

    std::string format_leave_room_command() const;

    std::string format_create_room_command() const;

    std::string format_delete_room_command() const;

    std::string format_message_command(const std::string& message) const;

    bool is_connected() const;

private:
    std::string server_ip;
    std::string server_port;
    std::string room_name;
    std::string name;
    std::string password;
    boost::asio::io_context io;
    boost::asio::streambuf buffer;
    tcp::socket socket;
    tcp::resolver resolver;
    std::vector<std::string> *log;
    std::vector<std::string> *chat;
    std::mutex *log_mutex;
    bool connected;
};

#endif //BASHMANGERCLIENT_CLIENT_H
