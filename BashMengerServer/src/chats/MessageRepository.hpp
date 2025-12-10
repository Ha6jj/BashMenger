#pragma once

#include "core/Types.hpp"

#include <deque>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>
#include <mutex>

struct StoredMessage
{
    uint64_t id;
    user_id_t sender_id;
    std::string content;
    std::chrono::system_clock::time_point timestamp;
};

class MessageRepository
{
public:
    static constexpr size_t MAX_STORED_MESSAGES = 1000;
    static constexpr size_t TRIM_THRESHOLD = MAX_STORED_MESSAGES + 100;

    void add_message(const user_id_t& sender_id, const std::string& content);
    std::vector<StoredMessage> get_missed_messages(const user_id_t& user_id);
    void mark_as_read(const user_id_t& user_id, uint64_t last_read_id);
    uint64_t get_latest_message_id() const noexcept;

private:
    mutable std::shared_mutex messages_mtx_;
    mutable std::shared_mutex users_mtx_;

    std::deque<StoredMessage> messages_;
    std::unordered_map<user_id_t, uint64_t> last_read_index_;

    uint64_t next_message_id_{1};
    std::atomic<uint64_t> latest_message_id_{0};

    uint64_t get_last_read_id(const user_id_t& user_id) const;
    
    void trim_old_messages_impl(std::unique_lock<std::shared_mutex>& message_lock);
};
