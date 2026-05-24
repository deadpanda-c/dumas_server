#include <gtest/gtest.h>

#include "Communication.hpp"

namespace {

TEST(SocketTests, PollWithoutBindingThrows) {
  Communication::Socket socket;
  EXPECT_THROW(socket.poll(0), Communication::Socket::SocketException);
}

TEST(SocketTests, BroadcastWithoutBindingThrows) {
  Communication::Socket socket;
  EXPECT_THROW(socket.broadcast("ping"), Communication::Socket::SocketException);
}

TEST(SocketTests, AcceptWithoutBindingThrows) {
  Communication::Socket socket;
  EXPECT_THROW(socket._accept(), Communication::Socket::SocketException);
}

TEST(SocketTests, DoubleBindThrows) {
  Communication::Socket socket;
  EXPECT_NO_THROW(socket._bind("127.0.0.1", 0));
  EXPECT_THROW(socket._bind("127.0.0.1", 0), Communication::Socket::SocketException);
}

}  // namespace
