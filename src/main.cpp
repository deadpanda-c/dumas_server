#include <iostream>
#include "Communication.hpp"

int main() {
  Communication::Socket server;

  try {
    server.init("127.0.0.1", 8080);
  } catch (const Communication::Socket::SocketException &e) {
    std::cerr << "Failed to initialize server: " << e.what() << std::endl;
    return 1;
  }

  bool running = true;
  while (running) {
    std::vector<Communication::Socket::IncomingMessage> messages;
    try {
      messages = server.poll(50);
    } catch (const Communication::Socket::SocketException &e) {
      std::cerr << "Failed while polling for messages: " << e.what() << std::endl;
      return 1;
    }

    for (const auto &message : messages) {
      std::cout << "Client " << message.client_fd << ": " << message.payload << std::endl;
    }
  }
  return 0;
}
