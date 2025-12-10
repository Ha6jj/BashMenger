#include "UserDataRepository.hpp"

#include <stdexcept>
#include <limits>
#include <mutex>

user_id_t UserDataRepository::register_user(const std::string& nickname, const std::string& password)
{
    if (nickname.empty() || password.empty()) 
    {
        return invalid_id;
    }

    std::unique_lock<std::shared_mutex> write_lock(repo_mutex);

    if (nickname_to_id.find(nickname) != nickname_to_id.end()) 
    {
        return invalid_id;
    }

    user_id_t new_id = generate_unique_id();
    if (new_id == invalid_id) {
        return invalid_id;
    }

    try
    {
        auto [requisites_it, requisites_success] = requisites.try_emplace(
            new_id, 
            UserData{nickname, std::hash<std::string>{}(password)}
        );

        if (!requisites_success)
        {
            throw std::runtime_error("Failed to insert user data");
        }

        auto [nickname_it, nickname_success] = nickname_to_id.try_emplace(
            nickname, 
            new_id
        );

        if (!nickname_success)
        {
            requisites.erase(requisites_it);
            throw std::runtime_error("Failed to insert nickname mapping");
        }

        return new_id;
    }
    catch (...)
    {
        return invalid_id;
    }
}

user_id_t UserDataRepository::authenticate(const std::string& nickname, const std::string& password) const
{
    std::shared_lock<std::shared_mutex> lock(repo_mutex);

    auto id_it = nickname_to_id.find(nickname);
    if (id_it == nickname_to_id.end())
    {
        return invalid_id;
    }

    user_id_t user_id = id_it->second;
    auto user_it = requisites.find(user_id);
    if (user_it == requisites.end())
    {
        return invalid_id;
    }

    return (user_it->second.password_hash == std::hash<std::string>{}(password)) ? user_id : invalid_id;
}

std::optional<std::string> UserDataRepository::get_nickname(user_id_t user_id) const
{
    std::shared_lock<std::shared_mutex> lock(repo_mutex);

    auto it = requisites.find(user_id);
    return (it != requisites.end()) 
        ? std::optional<std::string>(it->second.nickname) 
        : std::nullopt;
}

user_id_t UserDataRepository::generate_unique_id()
{
    constexpr int max_attempts = 100;
    int attempts = 0;

    while (attempts < max_attempts) {
        user_id_t current_id = next_user_id.load(std::memory_order_relaxed);

        if (current_id >= std::numeric_limits<user_id_t>::max() || current_id == 0) {
            user_id_t expected = current_id;
            next_user_id.compare_exchange_strong(
                expected, 
                1, 
                std::memory_order_relaxed,
                std::memory_order_relaxed
            );
            continue;
        }

        user_id_t expected = current_id;
        if (next_user_id.compare_exchange_strong(
            expected, 
            current_id + 1, 
            std::memory_order_relaxed,
            std::memory_order_relaxed
        )) {
            if (requisites.find(current_id) == requisites.end()) {
                return current_id;
            }
        }
        ++attempts;
    }
    return invalid_id;
}
