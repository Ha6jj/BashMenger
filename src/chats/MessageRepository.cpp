#include "MessageRepository.hpp"

#include <algorithm>
#include <thread>
#include <stdexcept>

void MessageRepository::add_message(const std::string& sender, const std::string& content)
{
    uint64_t new_id = next_message_id_.fetch_add(1, std::memory_order_relaxed);
    StoredMessage new_msg{
        new_id,
        sender,
        content,
        std::chrono::system_clock::now()
    };

    {
        std::lock_guard lock(messages_mtx_);
        messages_.push_back(std::move(new_msg));
        latest_message_id_.store(new_id, std::memory_order_release);
    }

    maybe_trim_old_messages();
}

std::vector<StoredMessage> MessageRepository::get_missed_messages(const std::string& username)
{
    uint64_t last_read_id;
    {
        std::shared_lock lock(users_mtx_);
        last_read_id = get_last_read_id(username);
    }

    std::shared_lock lock(messages_mtx_);
    auto it = std::lower_bound(messages_.begin(), messages_.end(), last_read_id,
        [](const StoredMessage& msg, uint64_t id)
        {
            return msg.id < id;
        });
    return std::vector<StoredMessage>(it, messages_.end());
}

void MessageRepository::mark_as_read(const std::string& username, uint64_t last_read_id)
{
    std::lock_guard lock(users_mtx_);
    auto& current_id = last_read_index_[username];
    if (last_read_id > current_id)
    {
        current_id = last_read_id;
    }
}

uint64_t MessageRepository::get_latest_message_id() const noexcept
{
    return latest_message_id_.load(std::memory_order_acquire);
}

uint64_t MessageRepository::get_last_read_id(const std::string& username) const
{
    auto it = last_read_index_.find(username);
    return (it != last_read_index_.end()) ? it->second : 0;
}

void MessageRepository::maybe_trim_old_messages()
{
    size_t current_size;
    {
        std::shared_lock lock(messages_mtx_);
        current_size = messages_.size();
    }
    if (current_size <= TRIM_THRESHOLD) return;

    std::thread([this] {
        trim_old_messages_impl();
    }).detach();
}

void MessageRepository::trim_old_messages_impl()
{
    uint64_t oldest_surviving_id = 0;
    {
        std::lock_guard lock(messages_mtx_);
        if (messages_.size() <= MAX_STORED_MESSAGES) return;
        
        size_t trim_pos = messages_.size() - MAX_STORED_MESSAGES;
        oldest_surviving_id = messages_[trim_pos].id;
        
        messages_.erase(messages_.begin(), messages_.begin() + trim_pos);
        
        if (!messages_.empty())
        {
            latest_message_id_.store(messages_.back().id, std::memory_order_release);
        }
        else
        {
            latest_message_id_.store(0, std::memory_order_release);
        }
    }

    std::lock_guard lock(users_mtx_);
    for (auto& [user, last_id] : last_read_index_)
    {
        if (last_id < oldest_surviving_id)
        {
            last_id = oldest_surviving_id;
        }
    }
}