#include <server_socket.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
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

    return true;
}

int ServerSocket::handleIncomingConnection()
{
    int new_socket = -1;
    try
    {
        new_socket = accept(m_server_fd, (struct sockaddr *)&m_address,
                            (socklen_t *)&m_address);
        if (new_socket < 0)
        {
            perror("accept");
        }
    }
    catch (...)
    {
        new_socket = -1;
        std::cout << "Error while reading from accept new socket\n";
    }

    return new_socket;
}

int ServerSocket::readMessage(int socketDescriptor, char *buffer, size_t length)
{
    assert(sizeof(buffer) < length);
    int byteRead = -1;
    try
    {
        byteRead = read(socketDescriptor, buffer, length);
    }
    catch (...)
    {
        byteRead = -1;
        std::cout << "Error while reading from socket\n";
    }
    return byteRead;
}

int ServerSocket::sendMessage(int socketDescriptor, const std::string &message)
{
    int byteSend = -1;
    try
    {
        byteSend = send(socketDescriptor, message.c_str(), message.size(), MSG_NOSIGNAL);
    }
    catch (...)
    {
        byteSend = -1;
        std::cout << "Error while sending to socket\n";
    }
    return byteSend;
}

int ServerSocket::closeConnection(int socketDescriptor)
{
    int status = -1;
    try
    {
        status = close(socketDescriptor);
    }
    catch (...)
    {
        status = -1;
        std::cout << "Error while closing the socket\n";
    }
    return status;
}