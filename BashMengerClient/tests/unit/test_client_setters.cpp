#include <gtest/gtest.h>
#include "../../Client.h"

class ClientSettersTest : public ::testing::Test {
protected:
    void SetUp() override {
        client = std::make_unique<Client>();
    }
    std::unique_ptr<Client> client;
};

// ========== Тесты setter'ов ==========

TEST_F(ClientSettersTest, SetServerIp) {
    Result result = client->setServerIp("192.168.1.1");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetServerIpLocalhost) {
    Result result = client->setServerIp("127.0.0.1");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetServerPort) {
    Result result = client->setServerPort("8080");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetServerPortDefault) {
    Result result = client->setServerPort("54001");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetRoomName) {
    Result result = client->setRoomName("general");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetRoomNameEmpty) {
    Result result = client->setRoomName("");
    EXPECT_EQ(result, OK);  // Клиент разрешает пустые значения
}

TEST_F(ClientSettersTest, SetName) {
    Result result = client->setName("Alice");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetNameWithNumbers) {
    Result result = client->setName("User123");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetPassword) {
    Result result = client->setPassword("secret123");
    EXPECT_EQ(result, OK);
}

TEST_F(ClientSettersTest, SetPasswordSpecialChars) {
    Result result = client->setPassword("p@$$w0rd!");
    EXPECT_EQ(result, OK);
}

// ========== Тесты граничных значений ==========

TEST_F(ClientSettersTest, SetEmptyValues) {
    EXPECT_EQ(client->setServerIp(""), OK);
    EXPECT_EQ(client->setServerPort(""), OK);
    EXPECT_EQ(client->setRoomName(""), OK);
    EXPECT_EQ(client->setName(""), OK);
    EXPECT_EQ(client->setPassword(""), OK);
}

TEST_F(ClientSettersTest, SetLongValues) {
    std::string long_string(1000, 'a');
    
    EXPECT_EQ(client->setServerIp(long_string), OK);
    EXPECT_EQ(client->setRoomName(long_string), OK);
    EXPECT_EQ(client->setName(long_string), OK);
    EXPECT_EQ(client->setPassword(long_string), OK);
}

TEST_F(ClientSettersTest, SetSpecialCharacters) {
    EXPECT_EQ(client->setName("Alice@123"), OK);
    EXPECT_EQ(client->setRoomName("room#1"), OK);
    EXPECT_EQ(client->setRoomName("test-room_2"), OK);
}

TEST_F(ClientSettersTest, SetUnicodeCharacters) {
    EXPECT_EQ(client->setName("Алиса"), OK);
    EXPECT_EQ(client->setRoomName("комната"), OK);
}

// ========== Тесты последовательности вызовов ==========

TEST_F(ClientSettersTest, SetMultipleTimesServerIp) {
    EXPECT_EQ(client->setServerIp("192.168.1.1"), OK);
    EXPECT_EQ(client->setServerIp("10.0.0.1"), OK);
    EXPECT_EQ(client->setServerIp("127.0.0.1"), OK);
}

TEST_F(ClientSettersTest, SetAllParametersSequentially) {
    EXPECT_EQ(client->setServerIp("127.0.0.1"), OK);
    EXPECT_EQ(client->setServerPort("54001"), OK);
    EXPECT_EQ(client->setName("Alice"), OK);
    EXPECT_EQ(client->setPassword("secret"), OK);
    EXPECT_EQ(client->setRoomName("general"), OK);
}

TEST_F(ClientSettersTest, ChangeParametersAfterSetting) {
    client->setName("Alice");
    EXPECT_EQ(client->setName("Bob"), OK);
    
    client->setPassword("old_pass");
    EXPECT_EQ(client->setPassword("new_pass"), OK);
}

// ========== Тесты валидации IP (если добавить валидацию) ==========

TEST_F(ClientSettersTest, SetInvalidIpFormat) {
    // Пока клиент принимает любые строки
    EXPECT_EQ(client->setServerIp("not.an.ip"), OK);
    EXPECT_EQ(client->setServerIp("999.999.999.999"), OK);
}

TEST_F(ClientSettersTest, SetInvalidPort) {
    // Пока клиент принимает любые строки
    EXPECT_EQ(client->setServerPort("99999"), OK);
    EXPECT_EQ(client->setServerPort("abc"), OK);
}

// ========== Тесты безопасности ==========

TEST_F(ClientSettersTest, SetPasswordWithSpaces) {
    EXPECT_EQ(client->setPassword("pass word"), OK);
}

TEST_F(ClientSettersTest, SetNameWithSpaces) {
    EXPECT_EQ(client->setName("Alice Bob"), OK);
}

TEST_F(ClientSettersTest, SetSqlInjectionAttempt) {
    // Клиент должен корректно обрабатывать потенциально опасные строки
    EXPECT_EQ(client->setName("'; DROP TABLE users; --"), OK);
}
