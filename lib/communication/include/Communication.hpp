/**
 * @file Communication.hpp
 * @brief Socket abstractions for basic TCP server operations.
 */
#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>
#include <vector>
#include <cstddef>
#include <sys/select.h>
#include <algorithm>

/** @brief Error message for binding an already-initialized socket. */
#define SOCKET_ALREADY_BOUND "Socket is already connected"
/** @brief Error message when socket creation fails. */
#define SOCKET_CREATION_FAILED "Failed to create socket"
/** @brief Error message when binding the socket fails. */
#define SOCKET_BIND_FAILED "Failed to bind socket"
/** @brief Error message for operations on an unbound socket. */
#define SOCKET_NOT_BOUND "Socket is not bound"
/** @brief Error message when accepting a client connection fails. */
#define SOCKET_ACCEPT_FAILED "Failed to accept connection"
/** @brief Error message when socket initialization fails. */
#define SOCKET_INITIALIZATION_FAILED "Failed to initialize socket"
/** @brief Error message when listening on the socket fails. */
#define SOCKET_LISTEN_FAILED "Failed to listen on socket"
/** @brief Error message when select fails. */
#define SOCKET_SELECT_FAILED "Failed to monitor socket events"
/** @brief Error message when receiving data from a client fails. */
#define SOCKET_SEND_FAILED "Failed to send message to client"
/** @brief Error message when closing a client connection fails. */
#define SOCKET_INVALID_CLIENT "Client not found or not connected"

namespace Communication {
  /**
   * @brief TCP server socket wrapper.
   *
   * Creates and binds a socket to an address and accepts incoming connections.
   * The socket is closed on destruction if it is open.
   */
  class Socket {
    public:
      /**
       * @brief Exception type thrown by socket operations.
       */
      class SocketException : public std::exception {
        public:
          /**
           * @brief Construct a new SocketException.
           * @param message Error message describing the failure.
           */
          SocketException(const std::string& message) : message(message) {}
          /**
           * @brief Return the error message.
           * @return Null-terminated error message string.
           */
          const char* what() const noexcept override {
            return message.c_str();
          }
        private:
          std::string message;
      };

      /**
       * @brief Construct a new Socket with no underlying descriptor.
       */
      Socket();
      /**
      * @brief Destroy the socket and close the descriptor if open.
      */
      ~Socket();

      /**
       * @brief Initialize the socket by creating and binding it to the given address.
       * @param ip IPv4 address to bind to.
       * @param port Port to bind to.
       * @return 0 on success.
       * @throws SocketException on failure to create or bind the socket.
       */
      int init(const std::string& ip, unsigned short port);


      /**
        * @brief Send a message to a connected client.
        * @param client_fd File descriptor of the client socket to send to.
        * @param message Message payload to send.
        * @return Number of bytes sent on success.
        * @throws SocketException if sending fails or the client is not connected.
        */
      int send(int client_fd, const std::string& message);

      /**
        * @brief Close a client connection and remove it from the list of connected clients.
        * @param client_fd File descriptor of the client socket to close.
        * @throws SocketException if the client is not found or closing fails.
        */
      void close_client(int client_fd);

      /**
       * @brief Create and bind the socket to the given address.
       * @param ip IPv4 address to bind to.
       * @param port Port to bind to.
       * @return 0 on success.
       * @throws SocketException on socket creation or bind failure.
       */
      int _bind(const std::string& ip, unsigned short port);
      /**
       * @brief Accept an incoming connection.
       * @return File descriptor for the accepted client socket.
       * @throws SocketException if the socket is not bound or accept fails.
       */
      int _accept();

      /**
       * @brief Incoming data received from a connected client.
       */
      struct IncomingMessage {
        int client_fd;
        std::string payload;
      };

      /**
       * @brief Poll for socket activity without blocking indefinitely.
       * @param timeout_ms Timeout in milliseconds. Use 0 for non-blocking.
       * @return Incoming messages read during this poll cycle.
       * @throws SocketException on select failure or invalid state.
       */
      std::vector<IncomingMessage> poll(int timeout_ms = 0);

      /**
       * @brief Broadcast a message to all connected clients.
       * @param message Message payload to send.
       * @throws SocketException if the server socket is not initialized.
       */
      void broadcast(const std::string& message);

    private:
      int _sockfd;
      std::vector<int> _connected_clients;
      fd_set _master_fds;
      int _max_fd;

      void _registerClient(int client_fd);
      void _disconnectClient(std::size_t index);
  };
}
