#include "Communication.hpp"

Communication::Socket::Socket() : _sockfd(-1)
{

}

int Communication::Socket::bind(const std::string &ip, unsigned short port)
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

int Communication::Socket::accept()
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

