#include "MessageRepository.hpp"

#include <algorithm>

void MessageRepository::add_message(const user_id_t& sender_id, const std::string& content)
{
    auto now = std::chrono::system_clock::now();
    std::unique_lock lock(messages_mtx_);

    uint64_t new_id = next_message_id_++;
    
    messages_.push_back(StoredMessage{
        new_id,
        sender_id,
        content,
        now
    });

    latest_message_id_.store(new_id, std::memory_order_release);

    if (messages_.size() > TRIM_THRESHOLD)
    {
        trim_old_messages_impl(lock);
    }
}

std::vector<StoredMessage> MessageRepository::get_missed_messages(const user_id_t& user_id)
{
    uint64_t last_read_id = 0;
    {
        std::shared_lock user_lock(users_mtx_);
        last_read_id = get_last_read_id(user_id);
    }

    std::shared_lock msg_lock(messages_mtx_);
    
    auto it = std::lower_bound(messages_.begin(), messages_.end(), last_read_id,
        [](const StoredMessage& msg, uint64_t id)
        {
            return msg.id < id;
        });
    return std::vector<StoredMessage>(it, messages_.end());
}

void MessageRepository::mark_as_read(const user_id_t& user_id, uint64_t last_read_id)
{
    std::unique_lock lock(users_mtx_);
    auto& current_id = last_read_index_[user_id];
    if (last_read_id > current_id)
    {
        current_id = last_read_id;
    }
}

uint64_t MessageRepository::get_latest_message_id() const noexcept
{
    return latest_message_id_.load(std::memory_order_acquire);
}

uint64_t MessageRepository::get_last_read_id(const user_id_t& user_id) const
{
    auto it = last_read_index_.find(user_id);
    return (it != last_read_index_.end()) ? it->second : 0;
}

void MessageRepository::trim_old_messages_impl(std::unique_lock<std::shared_mutex>& message_lock)
{
    if (messages_.size() <= MAX_STORED_MESSAGES) return;

    size_t trim_count = messages_.size() - MAX_STORED_MESSAGES;
    uint64_t oldest_surviving_id = messages_[trim_count].id;
    messages_.erase(messages_.begin(), messages_.begin() + trim_count);

    message_lock.unlock();

    {
        std::unique_lock user_lock(users_mtx_);
        for (auto& [user, last_id] : last_read_index_)
        {
            if (last_id < oldest_surviving_id)
            {
                last_id = oldest_surviving_id;
            }
        }
    }
}
