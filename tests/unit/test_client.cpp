#include <gtest/gtest.h>
#include "../../src/chats/core/Client.hpp"
#include "../test_helpers.hpp"

TEST(ClientTest, AuthorizeWithId) {
    auto client = std::make_shared<TestClient>();

    EXPECT_FALSE(client->is_authorized());

    client->authorize(42);

    EXPECT_TRUE(client->is_authorized());
    EXPECT_EQ(client->get_uid(), 42);
}

TEST(ClientTest, UniqueUIDs) {
    auto alice = std::make_shared<TestClient>();
    auto bob = std::make_shared<TestClient>();

    alice->authorize(1);
    bob->authorize(2);

    EXPECT_NE(alice->get_uid(), bob->get_uid());
}

TEST(ClientTest, UnauthorizedByDefault) {
    auto client = std::make_shared<TestClient>();

    EXPECT_FALSE(client->is_authorized());
}

TEST(ClientTest, DeliverToClient) {
    auto client = std::make_shared<TestClient>();

    client->deliver_to_client("Hello");
    client->deliver_to_client("World");

    ASSERT_EQ(client->GetMessages().size(), 2);
    EXPECT_EQ(client->GetMessages()[0], "Hello");
    EXPECT_EQ(client->GetMessages()[1], "World");
}

TEST(ClientTest, ClearMessages) {
    auto client = std::make_shared<TestClient>();

    client->deliver_to_client("Test");
    EXPECT_EQ(client->GetMessages().size(), 1);

    client->Clear();
    EXPECT_EQ(client->GetMessages().size(), 0);
}