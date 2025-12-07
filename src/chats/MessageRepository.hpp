#pragma once

#include <deque>
#include <string>
#include <unordered_map>
#include <chrono>
#include <shared_mutex>
#include <atomic>
#include <vector>

struct StoredMessage
{
    uint64_t id;
    std::string sender;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
};

class MessageRepository
{
public:
    static constexpr size_t MAX_STORED_MESSAGES = 1000;
    static constexpr size_t TRIM_THRESHOLD = MAX_STORED_MESSAGES + 100;

    void add_message(const std::string& sender, const std::string& content);
    std::vector<StoredMessage> get_missed_messages(const std::string& username);
    void mark_as_read(const std::string& username, uint64_t last_read_id);
    uint64_t get_latest_message_id() const noexcept;

private:
    mutable std::shared_mutex messages_mtx_;
    mutable std::shared_mutex users_mtx_;
    
    std::deque<StoredMessage> messages_;
    std::unordered_map<std::string, uint64_t> last_read_index_;
    
    std::atomic<uint64_t> next_message_id_{1};
    std::atomic<uint64_t> latest_message_id_{0};

    uint64_t get_last_read_id(const std::string& username) const;
    void maybe_trim_old_messages();
    void trim_old_messages_impl();
};
