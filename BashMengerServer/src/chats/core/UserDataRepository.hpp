#pragma once

#include "Types.hpp"

#include <string>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <shared_mutex>
#include <atomic>

struct UserData
{
    std::string nickname;
    size_t password_hash;
};

class UserDataRepository
{
public:
    static constexpr user_id_t invalid_id = 0;

    user_id_t register_user(const std::string& nickname, const std::string& password);
    user_id_t authenticate(const std::string& nickname, const std::string& password) const;
    std::optional<std::string> get_nickname(user_id_t user_id) const;

private:
    user_id_t generate_unique_id();

    mutable std::shared_mutex repo_mutex;
    std::unordered_map<user_id_t, UserData> requisites;
    std::unordered_map<std::string, user_id_t> nickname_to_id;
    std::atomic<user_id_t> next_user_id{1};
};
