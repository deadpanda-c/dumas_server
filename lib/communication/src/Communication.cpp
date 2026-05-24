#include "Communication.hpp"

Communication::Socket::Socket() : _sockfd(-1)
{

}

int Communication::Socket::init(const std::string &ip, unsigned short port)
{
  try {
    _bind(ip, port);
  } catch (const SocketException &e) {
    throw SocketException(SOCKET_INITIALIZATION_FAILED);
    return -1;
  }
  return 0;
}

void Communication::Socket::run()
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }

  if (listen(_sockfd, 5) == -1) {
    throw SocketException(SOCKET_LISTEN_FAILED);
  }

  std::cout << "Socket is listening for incoming connections..." << std::endl;
}

int Communication::Socket::_bind(const std::string &ip, unsigned short port)
{
  if (_sockfd != -1) {
    throw SocketException(SOCKET_ALREADY_BOUND);
  }

  _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd == -1) {
    throw SocketException(SOCKET_CREATION_FAILED);
  }

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

  if (::bind(_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
      close(_sockfd);
      _sockfd = -1;
      throw SocketException(SOCKET_BIND_FAILED);
  }

  std::cout << "Socket bound to " << ip << ":" << port << std::endl;
  return 0;
}

int Communication::Socket::_accept()
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }

  int client_sockfd = ::accept(_sockfd, nullptr, nullptr);
  if (client_sockfd == -1) {
    throw SocketException(SOCKET_ACCEPT_FAILED);
  }

  return client_sockfd;
}

