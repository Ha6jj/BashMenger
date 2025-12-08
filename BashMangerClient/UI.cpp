//
// Created by Feliks Rogachev on 08.12.2025.
//
#include "UI.h"

termios oldt;

void UI::run() {
    init_screen();
    while (true) {
        char c = getch();
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
            if (c == 27) {              // ESC
                mode = COMMAND;
            } else if (c == 127) {        // Backspace
                if (!input.empty())
                    input.pop_back();
            } else if (c == '\n') {       // Enter
                if (!input.empty()) {
                    chat.push_back("You: " + input);
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

UI::UI() {
    mode = COMMAND;
    std::string title = "";
    std::string ip = "";
    std::string port = "";
    std::string room = "";
    std::string password = "";
}

void UI::clear_creen() {
    std::cout << "\033[2J\033[H";
}

void UI::init_screen() {
    title = YELLOW + "Bash" + ORANGE + "Manger" + RED + " V1.0" + RESET;
    loadVars();
    createHelpList();
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

CMD UI::doCommand(std::string cmd) {
    std::istringstream line(cmd);
    std::string first;
    line >> first;
    if (first == "q") {
        return EXIT;
    } else if (first == "i") {
        mode = INSERT;
        return NOCMD;
    } else if (first == "ip") {
        std::string second;
        line >> second;
        ip = second;
        saveVars();
        cmd_story.push_back(GREEN + "added ip: " + RESET + second);
        return NOCMD;
    } else if (first == "port") {
        std::string second;
        line >> second;
        port = second;
        saveVars();
        cmd_story.push_back(GREEN + "added port: " + RESET + second);
        return NOCMD;
    } else if (first == "room") {
        std::string second;
        line >> second;
        room = second;
        saveVars();
        cmd_story.push_back(GREEN + "added room: " + RESET + second);
        return NOCMD;
    } else if (first == "pass") {
        std::string second;
        line >> second;
        password = second;
        saveVars();
        cmd_story.push_back(GREEN + "added password: " + RESET + second);
        return NOCMD;
    } else if (first == "h" || first == "help") {
        mode = HELP;
        cmd_story.push_back(GREEN + "opened: " + RESET + "Help page");
        return NOCMD;
    } else if (first == "p" || first == "prefs") {
        createVarList();
        mode = PREFS;
        cmd_story.push_back(GREEN + "opened: " + RESET + "Prefs page");
        return NOCMD;
    } else {
        cmd_story.push_back(RED + "No such command: " + cmd + RESET);
        return NOCMD;
    }
}

void UI::redraw() {
    int h = termH();
    int maxChatLines = h - 2;
    int start = 0;
    if (mode == COMMAND) {
        start = std::max(0, (int) cmd_story.size() - maxChatLines);
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
            if (start + i < cmd_story.size())
                std::cout << cmd_story[start + i];
        } else if (mode == HELP) {
            if (start + i < help_list.size())
                std::cout << help_list[start + i];
        } else if (mode == PREFS) {
            if (start + i < var_list.size())
                std::cout << var_list[start + i];
        } else {
            if (start + i < chat.size())
                std::cout << chat[start + i];
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
    file << password;
    file.close();
}