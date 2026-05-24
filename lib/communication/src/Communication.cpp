#include "Communication.hpp"
#include <cerrno>

Communication::Socket::Socket() : _sockfd(-1), _max_fd(-1)
{
  FD_ZERO(&_master_fds);
}

Communication::Socket::~Socket()
{
  for (int client_fd : _connected_clients) {
    close(client_fd);
  }
  _connected_clients.clear();
  if (_sockfd != -1) {
    close(_sockfd);
  }
}

int Communication::Socket::init(const std::string &ip, unsigned short port)
{
  try {
    _bind(ip, port);
    if (listen(_sockfd, 5) == -1) {
      throw SocketException(SOCKET_LISTEN_FAILED);
    }
    FD_ZERO(&_master_fds);
    FD_SET(_sockfd, &_master_fds);
    _max_fd = _sockfd;

  } catch (const SocketException &e) {
    throw SocketException(SOCKET_INITIALIZATION_FAILED);
    return -1;
  }
  return 0;
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

  _registerClient(client_sockfd);
  return client_sockfd;
}

std::vector<Communication::Socket::IncomingMessage> Communication::Socket::poll(int timeout_ms)
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }

  fd_set read_fds = _master_fds;
  timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;

  int ready = select(_max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
  if (ready == -1) {
    throw SocketException(SOCKET_SELECT_FAILED);
  }
  if (ready == 0) {
    return {};
  }

  if (FD_ISSET(_sockfd, &read_fds)) {
    _accept();
  }

  std::vector<IncomingMessage> messages;
  for (std::size_t i = 0; i < _connected_clients.size();) {
    int client_fd = _connected_clients[i];
    if (FD_ISSET(client_fd, &read_fds)) {
      char buffer[4096];
      ssize_t bytes = ::recv(client_fd, buffer, sizeof(buffer), 0);
      if (bytes == 0) {
        _disconnectClient(i);
        continue;
      }
      if (bytes < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) {
          _disconnectClient(i);
          continue;
        }
      }
      if (bytes > 0) {
        messages.push_back({client_fd, std::string(buffer, static_cast<std::size_t>(bytes))});
      }
    }
    ++i;
  }
  return messages;
}

int Communication::Socket::send(int client_fd, const std::string& message)
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }
  ssize_t sent = ::send(client_fd, message.c_str(), message.size(), 0);
  if (sent == -1) {
    throw SocketException(SOCKET_SEND_FAILED);
    return -1;
  }
  return static_cast<int>(sent);
}

void Communication::Socket::close_client(int client_fd)
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }
  auto it = std::find(_connected_clients.begin(), _connected_clients.end(), client_fd);
  if (it != _connected_clients.end()) {
    std::size_t index = static_cast<std::size_t>(std::distance(_connected_clients.begin(), it));
    _disconnectClient(index);
  } else {
    throw SocketException(SOCKET_INVALID_CLIENT);
  }
}


void Communication::Socket::broadcast(const std::string& message)
{
  if (_sockfd == -1) {
    throw SocketException(SOCKET_NOT_BOUND);
  }

  for (std::size_t i = 0; i < _connected_clients.size();) {
    int client_fd = _connected_clients[i];
    ssize_t sent = ::send(client_fd, message.c_str(), message.size(), 0);
    if (sent == -1) {
      _disconnectClient(i);
      continue;
    }
    ++i;
  }
}

void Communication::Socket::_registerClient(int client_fd)
{
  _connected_clients.push_back(client_fd);
  FD_SET(client_fd, &_master_fds);
  if (client_fd > _max_fd) {
    _max_fd = client_fd;
  }
}

void Communication::Socket::_disconnectClient(std::size_t index)
{
  int client_fd = _connected_clients[index];
  FD_CLR(client_fd, &_master_fds);
  close(client_fd);
  auto it = _connected_clients.begin()
    + static_cast<std::vector<int>::difference_type>(index);
  _connected_clients.erase(it);

  if (client_fd == _max_fd) {
    _max_fd = _sockfd;
    for (int fd : _connected_clients) {
      if (fd > _max_fd) {
        _max_fd = fd;
      }
    }
  }
}
