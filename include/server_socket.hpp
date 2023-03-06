#pragma once
#include <netinet/in.h>
#include <string>

class ServerSocket
{
private:
    struct sockaddr_in m_address;
    int m_server_fd{-1};
    int m_connection{-1};
    int m_port{8080};
    int m_backlog{10};
    bool m_is_started{false};

public:
    ServerSocket(int port, int backlog);
    ~ServerSocket();

    bool startListening();
    int acceptConnection();
    int readMessage(int socketDescriptor, char *buffer, size_t length);
    int sendMessage(int socketDescriptor, const std::string &message);
    int closeConnection(int socketDescriptor);
    bool isStarted() const;
};
