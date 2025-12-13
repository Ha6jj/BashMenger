#include <gtest/gtest.h>
#include "../../Client.h"

class ClientStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_unique<Client>();
    }
    std::unique_ptr<Client> client;
};

// ========== Тесты состояния ==========

TEST_F(ClientStateTest, InitiallyNotConnected) {
    EXPECT_FALSE(client->is_connected());
}

TEST_F(ClientStateTest, CanSetParametersWhenNotConnected) {
    EXPECT_FALSE(client->is_connected());
    
    EXPECT_EQ(client->setServerIp("127.0.0.1"), OK);
    EXPECT_EQ(client->setServerPort("54001"), OK);
    EXPECT_EQ(client->setName("Alice"), OK);
    EXPECT_EQ(client->setPassword("secret"), OK);
    EXPECT_EQ(client->setRoomName("general"), OK);
}

TEST_F(ClientStateTest, SettersReturnOK) {
    Result r1 = client->setServerIp("192.168.1.1");
    Result r2 = client->setServerPort("8080");
    Result r3 = client->setName("Bob");
    Result r4 = client->setPassword("pass");
    Result r5 = client->setRoomName("test");
    
    EXPECT_EQ(r1, OK);
    EXPECT_EQ(r2, OK);
    EXPECT_EQ(r3, OK);
    EXPECT_EQ(r4, OK);
    EXPECT_EQ(r5, OK);
}

TEST_F(ClientStateTest, MultipleSettersInSequence) {
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(client->setName("User" + std::to_string(i)), OK);
        EXPECT_EQ(client->setRoomName("Room" + std::to_string(i)), OK);
    }
}

TEST_F(ClientStateTest, SetSameValueMultipleTimes) {
    EXPECT_EQ(client->setName("Alice"), OK);
    EXPECT_EQ(client->setName("Alice"), OK);
    EXPECT_EQ(client->setName("Alice"), OK);
}

TEST_F(ClientStateTest, OverwritePreviousValues) {
    client->setName("Alice");
    EXPECT_EQ(client->setName("Bob"), OK);
    
    client->setPassword("old");
    EXPECT_EQ(client->setPassword("new"), OK);
    
    client->setRoomName("room1");
    EXPECT_EQ(client->setRoomName("room2"), OK);
}

TEST_F(ClientStateTest, CommandFormattingReflectsLatestValues) {
    client->setName("Alice");
    client->setPassword("pass1");
    std::string cmd1 = client->format_auth_command();
    
    client->setPassword("pass2");
    std::string cmd2 = client->format_auth_command();
    
    EXPECT_NE(cmd1, cmd2);
    EXPECT_EQ(cmd2, "cmd auth Alice pass2\n");
}
