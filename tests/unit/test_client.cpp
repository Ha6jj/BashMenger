#include <gtest/gtest.h>
#include "../../src/chats/core/Client.hpp"
#include "../test_helpers.hpp"

// Нет глобального users_count в новом проекте!
// Каждый Client сам генерирует UID

TEST(ClientTest, Authorize) {
    auto client = std::make_shared<TestClient>();
    EXPECT_FALSE(client->is_authorized());

    client->authorize("Alice");

    EXPECT_TRUE(client->is_authorized());
    EXPECT_EQ(client->get_nickname(), "Alice");
}

TEST(ClientTest, UniqueUIDs) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();

    alice->authorize("Alice");
    bob->authorize("Bob");

    EXPECT_NE(alice->get_uid(), bob->get_uid());
}