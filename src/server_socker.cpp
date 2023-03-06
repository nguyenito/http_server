#include <server_socket.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(int port, int backlog) : m_port(port), m_backlog(backlog)
{
}
ServerSocket::~ServerSocket()
{
    std::cout << "ServerSocket::Destructor closing listening socket on port " << m_port << std::endl;
    // closing the listening socket
    shutdown(m_server_fd, SHUT_RDWR);
}

bool ServerSocket::isStarted() const
{
    return m_is_started;
}

bool ServerSocket::startListening()
{
    m_is_started = false;

    // Creating socket file descriptor
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        return false;
    }

    int opt = 1;
    if (setsockopt(m_server_fd, SOL_SOCKET,
                   SO_REUSEADDR, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        return false;
    }

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    // Forcefully attaching socket to the port
    if (bind(m_server_fd, (struct sockaddr *)&m_address,
             sizeof(m_address)) < 0)
    {
        perror("bind failed");
        return false;
    }

    if (listen(m_server_fd, m_backlog) < 0)
    {
        perror("listen");
        return false;
    }
    m_is_started = true;
    std::cout << "Server Socker Started Listening On Port #" << m_port << std::endl;

    return true;
}

int ServerSocket::acceptConnection()
{
    int new_socket;
    if ((new_socket = accept(m_server_fd, (struct sockaddr *)&m_address,
                             (socklen_t *)&m_address)) < 0)
    {
        perror("accept");
    }
    return new_socket;
}

int ServerSocket::readMessage(int socketDescriptor, char *buffer, size_t length)
{
    assert(sizeof(buffer) < length);
    return read(socketDescriptor, buffer, length);
}

int ServerSocket::sendMessage(int socketDescriptor, const std::string &message)
{
    return send(socketDescriptor, message.c_str(), message.size(), 0);
}

int ServerSocket::closeConnection(int socketDescriptor)
{
    return close(socketDescriptor);
}