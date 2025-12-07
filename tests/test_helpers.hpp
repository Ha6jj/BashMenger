#pragma once

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "../src/chats/core/Client.hpp"

// Mock-клиент для тестирования без реальной сети
class TestClient : public Client {
public:
    TestClient() : Client(tcp::socket(io_context_)) {}

    void deliver_to_client(const std::string& msg) override {
        messages_.push_back(msg);
    }

    const std::vector<std::string>& GetMessages() const { return messages_; }
    void Clear() { messages_.clear(); }

private:
    static io::io_context io_context_;
    std::vector<std::string> messages_;
};