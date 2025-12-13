//
// Created by Feliks Rogachev on 08.12.2025.
//

#ifndef BASHMANGERCLIENT_UI_H
#define BASHMANGERCLIENT_UI_H

#include <string>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
#include <sstream>
#include <fstream>
#include "Client.h"
#include <fcntl.h>

enum Mode {
    COMMAND, INSERT, HELP, PREFS
};
enum CMD {
    EXIT, NOCMD
};

class UI {
public:
    UI();

    void run();

private:
    void clear_creen();

    void show_logo();

    void init_screen();

    void deinit_screen();

    void enableAltScreen();

    void enableRawMode();

    void disableRawMode();

    void disableAltScreen();

    void clearScreen();

    void redraw();

    void clearLine();

    void createHelpList();

    void createVarList();

    void saveVars();

    void loadVars();

    void setClientVars();

    void mutex_log_write(std::vector<std::string> &vec, std::string message);

    CMD doCommand(std::string cmd);

    char getch();

    int termH();

    void setCursor(int x, int y);

    Mode mode;
    int currentChat;
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string ORANGE = "\033[38;5;208m";
    std::vector<std::vector<std::string>> chat;
    std::vector<std::string> chat_meta;
    std::vector<std::string> cmd_story;
    std::vector<std::string> help_list;
    std::vector<std::string> var_list;
    std::string input;
    std::string title;
    std::string ip;
    std::string port;
    std::string room;
    std::string password;
    std::string name;
    std::mutex log_mutex;
    Client client;
};

#endif //BASHMANGERCLIENT_UI_H
