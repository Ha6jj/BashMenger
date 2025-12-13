#include <gtest/gtest.h>
#include "../../src/chats/RoomManager.hpp"
#include "../../src/chats/core/UserDataRepository.hpp"
#include "../test_helpers.hpp"

class RoomManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        user_repo_ = std::make_shared<UserDataRepository>();
        manager_ = std::make_unique<RoomManager>(*user_repo_);
    }
    
    std::shared_ptr<UserDataRepository> user_repo_;
    std::unique_ptr<RoomManager> manager_;
};

TEST_F(RoomManagerTest, CreateRoom) {
    manager_->create_room("test", 1);
    EXPECT_TRUE(manager_->room_exists("test"));
}

TEST_F(RoomManagerTest, RoomDoesNotExist) {
    EXPECT_FALSE(manager_->room_exists("nonexistent"));
}

TEST_F(RoomManagerTest, GetRoom) {
    manager_->create_room("test", 1);
    auto room = manager_->get_room("test");
    ASSERT_NE(room, nullptr);
}

TEST_F(RoomManagerTest, GetNonexistentRoom) {
    auto room = manager_->get_room("nonexistent");
    EXPECT_EQ(room, nullptr);
}

TEST_F(RoomManagerTest, RemoveRoom) {
    manager_->create_room("test", 1);
    EXPECT_TRUE(manager_->room_exists("test"));
    
    manager_->remove_room("test");
    EXPECT_FALSE(manager_->room_exists("test"));
}

TEST_F(RoomManagerTest, RemoveNonexistentRoom) {
    // Не должно крашиться
    manager_->remove_room("nonexistent");
    EXPECT_FALSE(manager_->room_exists("nonexistent"));
}

TEST_F(RoomManagerTest, MultipleRooms) {
    manager_->create_room("room1", 1);
    manager_->create_room("room2", 2);
    manager_->create_room("room3", 3);
    
    EXPECT_TRUE(manager_->room_exists("room1"));
    EXPECT_TRUE(manager_->room_exists("room2"));
    EXPECT_TRUE(manager_->room_exists("room3"));
}

TEST_F(RoomManagerTest, CreateDuplicateRoom) {
    manager_->create_room("test", 1);
    manager_->create_room("test", 2);  // Попытка создать дубликат
    
    auto room = manager_->get_room("test");
    ASSERT_NE(room, nullptr);
    EXPECT_TRUE(room->is_admin(1));  // Админ первый, не второй
}
