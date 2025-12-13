#include <gtest/gtest.h>
#include "../../src/chats/ChatRoom.hpp"
#include "../../src/chats/core/UserDataRepository.hpp"
#include "../test_helpers.hpp"

class ChatRoomTest : public ::testing::Test {
protected:
    void SetUp() override {
        user_repo_ = std::make_shared<UserDataRepository>();
        
        // Регистрируем тестовых пользователей
        alice_id_ = user_repo_->register_user("Alice", "pass1");
        bob_id_ = user_repo_->register_user("Bob", "pass2");
        charlie_id_ = user_repo_->register_user("Charlie", "pass3");
        
        room_ = std::make_shared<ChatRoom>(alice_id_, "test_room", *user_repo_);
    }
    
    std::shared_ptr<UserDataRepository> user_repo_;
    std::shared_ptr<ChatRoom> room_;
    user_id_t alice_id_;
    user_id_t bob_id_;
    user_id_t charlie_id_;
};

TEST_F(ChatRoomTest, JoinParticipant) {
    auto alice = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    
    room_->join_participant(alice);
    
    EXPECT_TRUE(room_->is_member(alice_id_));
}

TEST_F(ChatRoomTest, LeaveParticipant) {
    auto alice = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    
    room_->join_participant(alice);
    EXPECT_TRUE(room_->is_member(alice_id_));
    
    room_->leave_participant(alice);
    EXPECT_FALSE(room_->is_member(alice_id_));
}

TEST_F(ChatRoomTest, BroadcastExcludesSender) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    bob->authorize(bob_id_);
    
    room_->join_participant(alice);
    room_->join_participant(bob);
    alice->Clear();
    bob->Clear();
    
    room_->broadcast("test message\n", alice);
    
    EXPECT_EQ(alice->GetMessages().size(), 0);  // Alice excluded
    EXPECT_EQ(bob->GetMessages().size(), 1);    // Bob receives
    EXPECT_EQ(bob->GetMessages()[0], "test message\n");
}

TEST_F(ChatRoomTest, BroadcastToAll) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    bob->authorize(bob_id_);
    
    room_->join_participant(alice);
    room_->join_participant(bob);
    alice->Clear();
    bob->Clear();
    
    room_->broadcast("announcement\n", nullptr);  // No exclusion
    
    EXPECT_EQ(alice->GetMessages().size(), 1);
    EXPECT_EQ(bob->GetMessages().size(), 1);
}

TEST_F(ChatRoomTest, IsAdmin) {
    EXPECT_TRUE(room_->is_admin(alice_id_));   // Alice is admin
    EXPECT_FALSE(room_->is_admin(bob_id_));    // Bob is not
}

TEST_F(ChatRoomTest, JoinNotification) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    bob->authorize(bob_id_);
    
    room_->join_participant(alice);
    alice->Clear();
    
    room_->join_participant(bob);
    
    // Alice should receive join notification
    EXPECT_GT(alice->GetMessages().size(), 0);
    
    bool found_join_message = false;
    for (const auto& msg : alice->GetMessages()) {
        if (msg.find("Participant joined: Bob") != std::string::npos) {
            found_join_message = true;
            break;
        }
    }
    EXPECT_TRUE(found_join_message);
}

TEST_F(ChatRoomTest, LeaveNotification) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    bob->authorize(bob_id_);
    
    room_->join_participant(alice);
    room_->join_participant(bob);
    alice->Clear();
    bob->Clear();
    
    room_->leave_participant(bob);
    
    // Alice should receive leave notification
    EXPECT_GT(alice->GetMessages().size(), 0);
    
    bool found_leave_message = false;
    for (const auto& msg : alice->GetMessages()) {
        if (msg.find("Participant left: Bob") != std::string::npos) {
            found_leave_message = true;
            break;
        }
    }
    EXPECT_TRUE(found_leave_message);
}

TEST_F(ChatRoomTest, SubmitMessage) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    bob->authorize(bob_id_);
    
    room_->join_participant(alice);
    room_->join_participant(bob);
    alice->Clear();
    bob->Clear();
    
    room_->submit_message("Hello everyone!", alice);
    
    // Alice shouldn't receive her own message (broadcast excludes sender)
    EXPECT_EQ(alice->GetMessages().size(), 0);
    
    // Bob should receive the message with Alice's nickname
    ASSERT_EQ(bob->GetMessages().size(), 1);
    EXPECT_TRUE(bob->GetMessages()[0].find("Alice:") != std::string::npos);
    EXPECT_TRUE(bob->GetMessages()[0].find("Hello everyone!") != std::string::npos);
}

TEST_F(ChatRoomTest, NonMemberCannotSendMessage) {
    auto bob = std::make_shared<TestClient>();
    bob->authorize(bob_id_);
    
    // Bob not a member
    bob->Clear();
    room_->submit_message("Hello", bob);
    
    // Bob should receive error message
    ASSERT_EQ(bob->GetMessages().size(), 1);
    EXPECT_TRUE(bob->GetMessages()[0].find("aren't member") != std::string::npos);
}

TEST_F(ChatRoomTest, AddAndRemoveSession) {
    auto alice = std::make_shared<TestClient>();
    alice->authorize(alice_id_);
    
    room_->add_new_session(alice);
    // Session added (no direct way to test, but shouldn't crash)
    
    room_->remove_session(alice);
    // Session removed (no direct way to test, but shouldn't crash)
}

TEST_F(ChatRoomTest, MultipleSessionsSameUser) {
    auto alice_session1 = std::make_shared<TestClient>();
    auto alice_session2 = std::make_shared<TestClient>();
    alice_session1->authorize(alice_id_);
    alice_session2->authorize(alice_id_);
    
    room_->join_participant(alice_session1);
    room_->add_new_session(alice_session2);
    alice_session1->Clear();
    alice_session2->Clear();
    
    // Broadcast to both sessions
    room_->broadcast("test\n", nullptr);
    
    EXPECT_EQ(alice_session1->GetMessages().size(), 1);
    EXPECT_EQ(alice_session2->GetMessages().size(), 1);
}
