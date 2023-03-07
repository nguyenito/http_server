#pragma once
#include <netinet/in.h>
#include <string>
#include <vector>

class ServerSocket
{
private:
    bool m_is_started{false};
    int m_server_fd{-1};
    int m_connection{-1};
    int m_port{8080};
    int m_backlog{10};
    int m_master_socket{-1};
    struct sockaddr_in m_address;
    struct sockaddr_in m_cli_addr;
    std::vector<int> m_client_socket;

public:
    ServerSocket(int port, int backlog);
    ~ServerSocket();

    bool startListening();
    int handleIncomingConnection();
    int readMessage(int socketDescriptor, char *buffer, size_t length);
    int sendMessage(int socketDescriptor, const std::string &message);
    int closeConnection(int socketDescriptor);
    bool isStarted() const;
};
