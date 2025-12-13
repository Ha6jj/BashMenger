#include <gtest/gtest.h>
#include "../../src/chats/core/UserDataRepository.hpp"
#include "../test_helpers.hpp"

class UserDataRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        repo_ = std::make_unique<UserDataRepository>();
    }
    std::unique_ptr<UserDataRepository> repo_;
};

TEST_F(UserDataRepositoryTest, RegisterUser) {
    user_id_t id = repo_->register_user("Alice", "password123");
    
    EXPECT_NE(id, UserDataRepository::invalid_id);
    EXPECT_GT(id, 0);
}

TEST_F(UserDataRepositoryTest, RegisterDuplicateNickname) {
    user_id_t id1 = repo_->register_user("Alice", "password1");
    user_id_t id2 = repo_->register_user("Alice", "password2");
    
    EXPECT_NE(id1, UserDataRepository::invalid_id);
    EXPECT_EQ(id2, UserDataRepository::invalid_id);
}

TEST_F(UserDataRepositoryTest, RegisterEmptyNickname) {
    user_id_t id = repo_->register_user("", "password");
    EXPECT_EQ(id, UserDataRepository::invalid_id);
}

TEST_F(UserDataRepositoryTest, RegisterEmptyPassword) {
    user_id_t id = repo_->register_user("Alice", "");
    EXPECT_EQ(id, UserDataRepository::invalid_id);
}

TEST_F(UserDataRepositoryTest, AuthenticateSuccess) {
    user_id_t register_id = repo_->register_user("Alice", "password123");
    user_id_t auth_id = repo_->authenticate("Alice", "password123");
    
    EXPECT_EQ(register_id, auth_id);
}

TEST_F(UserDataRepositoryTest, AuthenticateWrongPassword) {
    repo_->register_user("Alice", "password123");
    user_id_t auth_id = repo_->authenticate("Alice", "wrong_password");
    
    EXPECT_EQ(auth_id, UserDataRepository::invalid_id);
}

TEST_F(UserDataRepositoryTest, AuthenticateNonexistentUser) {
    user_id_t auth_id = repo_->authenticate("Bob", "password");
    EXPECT_EQ(auth_id, UserDataRepository::invalid_id);
}

TEST_F(UserDataRepositoryTest, GetNickname) {
    user_id_t id = repo_->register_user("Alice", "password");
    auto nickname = repo_->get_nickname(id);
    
    ASSERT_TRUE(nickname.has_value());
    EXPECT_EQ(*nickname, "Alice");
}

TEST_F(UserDataRepositoryTest, GetNicknameInvalidId) {
    auto nickname = repo_->get_nickname(999);
    EXPECT_FALSE(nickname.has_value());
}

TEST_F(UserDataRepositoryTest, UniqueIds) {
    user_id_t id1 = repo_->register_user("Alice", "pass1");
    user_id_t id2 = repo_->register_user("Bob", "pass2");
    user_id_t id3 = repo_->register_user("Charlie", "pass3");
    
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}
