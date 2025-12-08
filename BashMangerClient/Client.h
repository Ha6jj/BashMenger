//
// Created by Feliks Rogachev on 08.12.2025.
//
#ifndef BASHMANGERCLIENT_CLIENT_H
#define BASHMANGERCLIENT_CLIENT_H

#include <string>

class Client {
public:
    Client(std::string ip, std::string port, std::string room);
    void run();
private:
    std::string server_ip;
    std::string server_port;
    std::string room_number;
};

#endif //BASHMANGERCLIENT_CLIENT_H
