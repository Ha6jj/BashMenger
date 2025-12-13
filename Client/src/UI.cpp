//
// Created by Feliks Rogachev on 08.12.2025.
//
#include "UI.h"

termios oldt;

void UI::run() {
    init_screen();

    while (true) {
        char c = getch();
        bool isArrow = false;
        char a1 = 0, a2 = 0;
        if (mode == COMMAND) {
            if (c == '\n') {       // Enter
                if (!input.empty()) {
                    if (doCommand(input) == EXIT) {
                        break;
                    };
                    input.clear();
                }
            } else if (c == 127) {        // Backspace
                if (!input.empty())
                    input.pop_back();
            } else {                    // Regular
                input.push_back(c);
            }

        } else if (mode == INSERT) {
            if (c == 27) {
                if (read(STDIN_FILENO, &a1, 1) == 1 && read(STDIN_FILENO, &a2, 1) == 1) {
                    if (a1 == '[') {
                        if (a2 == 'C') {
                            currentChat = (currentChat + 1) % (chat.size());
                        }
                        if (a2 == 'D') {
                            currentChat = (currentChat - 1) % (chat.size());
                        }
                    } else {
                        mode = COMMAND;
                    }
                } else {
                    mode = COMMAND;
                }
            } else if (c == 127) {        // Backspace
                if (!input.empty())
                    input.pop_back();
            } else if (c == '\n') {       // Enter
                if (!input.empty()) {
                    mutex_log_write(chat[currentChat], "You: " + input);
                    if (currentChat != 0) {
                        client.send_message(input);
                    }
                    input.clear();
                }
            } else {                       // Regular
                input.push_back(c);
            }
        } else if (mode == HELP) {
            if (c == 27) {              // ESC
                mode = COMMAND;
            }
        } else if (mode == PREFS) {
            if (c == 27) {              // ESC
                mode = COMMAND;
            }

        }

        redraw();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    deinit_screen();
}

void UI::show_logo() {
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string ORANGE = "\033[38;5;208m";
    std::cout << RED
              << "===========================================================================================================\n";
    std::cout << ORANGE << " /$$$$$$$                      /$$     " << YELLOW
              << "      /$$      /$$                                                  \n";
    std::cout << ORANGE << "| $$__  $$                    | $$      " << YELLOW
              << "    | $$$    /$$$                                                  \n";
    std::cout << ORANGE << "| $$  \\ $$  /$$$$$$   /$$$$$$$| $$$$$$$  " << YELLOW
              << "   | $$$$  /$$$$  /$$$$$$  /$$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$ \n";
    std::cout << ORANGE << "| $$$$$$$  |____  $$ /$$_____/| $$__  $$   " << YELLOW
              << " | $$ $$/$$ $$ |____  $$| $$__  $$ /$$__  $$ /$$__  $$ /$$__  $$\n";
    std::cout << ORANGE << "| $$__  $$  /$$$$$$$|  $$$$$$ | $$  \\ $$  " << YELLOW
              << "  | $$  $$$| $$  /$$$$$$$| $$  \\ $$| $$  \\ $$| $$$$$$$$| $$  \\__/\n";
    std::cout << ORANGE << "| $$  \\ $$ /$$__  $$ \\____  $$| $$  | $$ " << YELLOW
              << "   | $$\\  $ | $$ /$$__  $$| $$  | $$| $$  | $$| $$_____/| $$      \n";
    std::cout << ORANGE << "| $$$$$$$/|  $$$$$$$ /$$$$$$$/| $$  | $$   " << YELLOW
              << " | $$ \\/  | $$|  $$$$$$$| $$  | $$|  $$$$$$$|  $$$$$$$| $$      \n";
    std::cout << ORANGE << "|_______/  \\_______/|_______/ |__/  |__/  " << YELLOW
              << "  |__/     |__/ \\_______/|__/  |__/ \\____  $$ \\_______/|__/      \n";
    std::cout << ORANGE << "                                           " << YELLOW
              << "                                    /$$  \\ $$                    \n";
    std::cout << RED << "==============================================================================" << YELLOW
              << "|  $$$$$$/" << RED << "===================\n";
    std::cout << ORANGE << "                                           " << YELLOW
              << "                                    \\______/                     \n";
    return;
}

UI::UI() : client() {
    mode = COMMAND;
    title = "";
    ip = "";
    port = "";
    room = "";
    password = "";
    chat = {std::vector<std::string>()};
    chat[0].push_back(ORANGE + "Chat: " + RESET + "saved messages");
    chat_meta = {"saved messages"};
    currentChat = 0;
    loadVars();
    createHelpList();
    setClientVars();
    client.onMessageReceived = [this]() {
        redraw();
    };
}

void UI::clear_creen() {
    std::cout << "\033[2J\033[H";
}

void UI::init_screen() {
    title = YELLOW + "Bash" + ORANGE + "Manger" + RED + " V1.0" + RESET;
    enableAltScreen();
    enableRawMode();
    redraw();
}

void UI::deinit_screen() {
    disableRawMode();
    disableAltScreen();
    clearScreen();
}


char UI::getch() {
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}


void UI::enableRawMode() {
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void UI::disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void UI::enableAltScreen() {
    std::cout << "\033[?1049h";  // альтернативный экран
    std::cout << "\033[?25l";    // скрыть курсор
}


void UI::disableAltScreen() {
    std::cout << "\033[?25h";    // показать курсор
    std::cout << "\033[?1049l";  // вернуть обычный экран
}


int UI::termH() {
    winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

void UI::setCursor(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

void UI::clearLine() {
    std::cout << "\033[2K";
}

void UI::clearScreen() {
    std::cout << "\033[2J";
}

void UI::setClientVars() {
    client.setServerIp(ip);
    client.setServerPort(port);
    client.setPassword(password);
    client.setName(name);
    client.setRoomName(room);
}

CMD UI::doCommand(std::string cmd) {
    std::istringstream line(cmd);
    std::string first;
    line >> first;
    if (first == "q") {
        client.disconnect();
        return EXIT;
    } else if (first == "i") {
        mode = INSERT;
        return NOCMD;
    } else if (first == "ip") {
        std::string second;
        line >> second;
        ip = second;
        saveVars();
        setClientVars();
        mutex_log_write(cmd_story, GREEN + "added ip: " + RESET + second);
        return NOCMD;
    } else if (first == "port") {
        std::string second;
        line >> second;
        port = second;
        saveVars();
        setClientVars();
        mutex_log_write(cmd_story, GREEN + "added port: " + RESET + second);
        return NOCMD;
    } else if (first == "room") {
        std::string second;
        line >> second;
        room = second;
        saveVars();
        setClientVars();
        mutex_log_write(cmd_story, GREEN + "added room: " + RESET + second);
        return NOCMD;
    } else if (first == "name") {
        std::string second;
        line >> second;
        name = second;
        saveVars();
        setClientVars();
        mutex_log_write(cmd_story, GREEN + "added name: " + RESET + second);
        return NOCMD;
    } else if (first == "pass") {
        std::string second;
        line >> second;
        password = second;
        saveVars();
        setClientVars();
        mutex_log_write(cmd_story, GREEN + "added password: " + RESET + second);
        return NOCMD;
    } else if (first == "h" || first == "help") {
        input.clear();
        mode = HELP;
        mutex_log_write(cmd_story, GREEN + "opened: " + RESET + "Help page");
        return NOCMD;
    } else if (first == "p" || first == "prefs") {
        input.clear();
        createVarList();
        mode = PREFS;
        mutex_log_write(cmd_story, GREEN + "opened: " + RESET + "Prefs page");
        return NOCMD;
    } else if (first == "connect") {
        chat.push_back(std::vector<std::string>{ORANGE + "Chat: " + RESET + room});
        chat_meta.push_back(room);
        currentChat = chat.size() - 1;
        client.connect_to_server(&cmd_story, &chat[currentChat], &log_mutex);
        mutex_log_write(cmd_story, GREEN + "connected to: " + RESET + ip + ":" + port);
        return NOCMD;
    } else if (first == "disconnect") {
        client.disconnect();
        mutex_log_write(cmd_story, GREEN + "disconnected from: " + RESET + ip + ":" + port);
        return NOCMD;
    } else if (first == "join") {
        client.join_room();
        mutex_log_write(cmd_story, GREEN + "joined: " + RESET + room);
        return NOCMD;
    } else if (first == "leave") {
        client.leave_room();
        mutex_log_write(cmd_story, GREEN + "leaved: " + RESET + room);
        return NOCMD;
    } else if (first == "delete") {
        client.del_room();
        mutex_log_write(cmd_story, GREEN + "deleted: " + RESET + room);
        return NOCMD;
    } else if (first == "register") {
        client.reg_acc();
        mutex_log_write(cmd_story, GREEN + "registered new account: " + RESET + name);
        return NOCMD;
    } else if (first == "auth") {
        client.auth_acc();
        mutex_log_write(cmd_story, GREEN + "entered account: " + RESET + name);
        return NOCMD;
    } else if (first == "leave") {
        client.leave_room();
        mutex_log_write(cmd_story, GREEN + "leaved: " + RESET + room);
        return NOCMD;
    } else if (first == "create") {
        client.create_room();
        mutex_log_write(cmd_story, GREEN + "created new room: " + RESET + room);
        return NOCMD;
    } else {
        mutex_log_write(cmd_story, RED + "No such command: " + cmd + RESET);
        return NOCMD;
    }
}

void UI::redraw() {
    int h = termH();
    int maxChatLines = h - 2;
    int start = 0;
    std::vector<std::string> snapshot;
    if (mode == COMMAND) {
        log_mutex.lock();
        snapshot = cmd_story;
        log_mutex.unlock();
        start = std::max(0, (int) snapshot.size() - maxChatLines);
    } else if (mode == HELP) {
        start = std::max(0, (int) help_list.size() - maxChatLines);
    } else if (mode == PREFS) {
        start = std::max(0, (int) var_list.size() - maxChatLines);
    } else {
        start = std::max(0, (int) chat.size() - maxChatLines);
    }

    setCursor(1, 1);
    clearLine();
    std::cout << title;
    for (int i = 0; i < maxChatLines; ++i) {
        setCursor(1, i + 2);
        clearLine();
        if (mode == COMMAND) {
            if (start + i < snapshot.size())
                std::cout << snapshot[start + i];
        } else if (mode == HELP) {
            if (start + i < help_list.size())
                std::cout << help_list[start + i];
        } else if (mode == PREFS) {
            if (start + i < var_list.size())
                std::cout << var_list[start + i];
        } else {
            if (start + i < chat[currentChat].size())
                std::cout << chat[currentChat][start + i];
        }

    }

    setCursor(1, h);
    clearLine();
    if (mode == COMMAND)
        std::cout << "COMMAND: ";
    else if (mode == HELP)
        std::cout << "HELP. ";
    else if (mode == PREFS)
        std::cout << "PREFS. ";
    else
        std::cout << "INSERT: ";
    std::cout << input << std::flush;
}

void UI::createHelpList() {
    help_list = {};
    std::ifstream file("Help.txt");
    if (!file) {
        help_list.push_back(RED + "Could not load Help page" + RESET);
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        help_list.push_back(line);
    }
    file.close();
    return;
}

void UI::createVarList() {
    var_list = {};
    var_list.push_back("All your saved prefs:");
    var_list.push_back(" " + GREEN + "ip: " + RESET + ip);
    var_list.push_back(" " + GREEN + "port: " + RESET + port);
    var_list.push_back(" " + GREEN + "room: " + RESET + room);
    var_list.push_back(" " + GREEN + "password: " + RESET + password);
    var_list.push_back(" " + GREEN + "name: " + RESET + name);
    var_list.push_back("press ESC to leave");
    return;
}

void UI::loadVars() {
    std::ifstream file("data.txt");
    if (!file) {
        return;
    }
    std::string place;
    if (std::getline(file, place)) {
        ip = place.substr(0, 10);
    }
    if (std::getline(file, place)) {
        port = place.substr(0, 10);
    }
    if (std::getline(file, place)) {
        room = place.substr(0, 10);
    }
    if (std::getline(file, place)) {
        password = place.substr(0, 10);
    }
    if (std::getline(file, place)) {
        name = place.substr(0, 10);
    }
    file.close();
    return;
}

void UI::saveVars() {
    std::ofstream file("data.txt");
    if (!file) {
        return;
    }
    file << ip << "\n";
    file << port << "\n";
    file << room << "\n";
    file << password << "\n";
    file << name;
    file.close();
}

void UI::mutex_log_write(std::vector<std::string> &vec, std::string message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    vec.push_back(message);
}