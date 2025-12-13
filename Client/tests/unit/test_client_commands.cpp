#include <gtest/gtest.h>
#include "../../src/Client.h"

class ClientCommandsTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_unique<Client>();
        client->setName("Alice");
        client->setPassword("secret123");
        client->setRoomName("general");
    }
    std::unique_ptr<Client> client;
};

// ========== Тесты форматирования команд ==========

TEST_F(ClientCommandsTest, FormatAuthCommand) {
    std::string cmd = client->format_auth_command();
    EXPECT_EQ(cmd, "cmd auth Alice secret123\n");
}

TEST_F(ClientCommandsTest, FormatRegisterCommand) {
    std::string cmd = client->format_register_command();
    EXPECT_EQ(cmd, "cmd register Alice secret123\n");
}

TEST_F(ClientCommandsTest, FormatJoinRoomCommand) {
    std::string cmd = client->format_join_room_command();
    EXPECT_EQ(cmd, "cmd join general\n");
}

TEST_F(ClientCommandsTest, FormatLeaveRoomCommand) {
    std::string cmd = client->format_leave_room_command();
    EXPECT_EQ(cmd, "cmd leave general\n");
}

TEST_F(ClientCommandsTest, FormatCreateRoomCommand) {
    std::string cmd = client->format_create_room_command();
    EXPECT_EQ(cmd, "cmd create general\n");
}

TEST_F(ClientCommandsTest, FormatDeleteRoomCommand) {
    std::string cmd = client->format_delete_room_command();
    EXPECT_EQ(cmd, "cmd delete general\n");
}

TEST_F(ClientCommandsTest, FormatMessageCommand) {
    std::string cmd = client->format_message_command("Hello World");
    EXPECT_EQ(cmd, "msg general Hello World\n");
}

TEST_F(ClientCommandsTest, FormatMessageWithSpecialChars) {
    std::string cmd = client->format_message_command("Hello! @user #tag");
    EXPECT_EQ(cmd, "msg general Hello! @user #tag\n");
}

TEST_F(ClientCommandsTest, FormatMessageEmpty) {
    std::string cmd = client->format_message_command("");
    EXPECT_EQ(cmd, "msg general \n");
}

TEST_F(ClientCommandsTest, FormatCommandsWithEmptyName) {
    client->setName("");
    
    std::string auth = client->format_auth_command();
    EXPECT_EQ(auth, "cmd auth  secret123\n");
    
    std::string reg = client->format_register_command();
    EXPECT_EQ(reg, "cmd register  secret123\n");
}

TEST_F(ClientCommandsTest, FormatCommandsWithEmptyPassword) {
    client->setPassword("");
    
    std::string auth = client->format_auth_command();
    EXPECT_EQ(auth, "cmd auth Alice \n");
}

TEST_F(ClientCommandsTest, FormatCommandsWithEmptyRoom) {
    client->setRoomName("");
    
    std::string join = client->format_join_room_command();
    EXPECT_EQ(join, "cmd join \n");
    
    std::string msg = client->format_message_command("test");
    EXPECT_EQ(msg, "msg  test\n");
}

TEST_F(ClientCommandsTest, FormatCommandsWithSpaces) {
    client->setName("Alice Bob");
    client->setPassword("pass word");
    client->setRoomName("test room");
    
    std::string auth = client->format_auth_command();
    EXPECT_EQ(auth, "cmd auth Alice Bob pass word\n");
    
    std::string join = client->format_join_room_command();
    EXPECT_EQ(join, "cmd join test room\n");
}

TEST_F(ClientCommandsTest, FormatCommandsWithUnicode) {
    client->setName("Алиса");
    client->setRoomName("комната");
    
    std::string auth = client->format_auth_command();
    EXPECT_EQ(auth, "cmd auth Алиса secret123\n");
    
    std::string join = client->format_join_room_command();
    EXPECT_EQ(join, "cmd join комната\n");
}

TEST_F(ClientCommandsTest, FormatMessageMultiline) {
    std::string multiline = "Line1\nLine2\nLine3";
    std::string cmd = client->format_message_command(multiline);
    EXPECT_EQ(cmd, "msg general Line1\nLine2\nLine3\n");
}

TEST_F(ClientCommandsTest, FormatAllCommandsSequentially) {
    // Проверяем что можно вызвать все команды подряд
    EXPECT_NO_THROW(client->format_auth_command());
    EXPECT_NO_THROW(client->format_register_command());
    EXPECT_NO_THROW(client->format_join_room_command());
    EXPECT_NO_THROW(client->format_leave_room_command());
    EXPECT_NO_THROW(client->format_create_room_command());
    EXPECT_NO_THROW(client->format_delete_room_command());
    EXPECT_NO_THROW(client->format_message_command("test"));
}
