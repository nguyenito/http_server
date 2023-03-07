// Client side C / C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <future>
#include <vector>
#include <http_request.hpp>
#define PORT 8080

std::string TEST_FILE = "/Users/npbnguyen/Desktop/Learning/dtl/http-server/test/http_test";

class ClientSocket
{
private:
    bool m_is_connected;
    int m_client_fd;
    int m_port;

public:
    ClientSocket(int port) : m_is_connected(false), m_client_fd(-1), m_port(port) {}
    ~ClientSocket()
    {
        if (m_is_connected)
        {
            close(m_client_fd);
        }
    }

    bool connectServer()
    {
        m_is_connected = false;
        int status, valread;
        struct sockaddr_in serv_addr;
        bool success = false;
        if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            usleep(100000);
            return false;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(m_port);

        // Convert IPv4 and IPv6 addresses from text to binary
        // form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            printf(
                "\nInvalid address/ Address not supported \n");
            usleep(100000);
            return false;
        }

        if ((status = connect(m_client_fd, (struct sockaddr *)&serv_addr,
                              sizeof(serv_addr))) < 0)
        {
            return false;
        }

        m_is_connected = true;
        return true;
    }

    bool sendMessage(const std::string &message)
    {
        if (!m_is_connected)
            return false;

        int sendRes = send(m_client_fd, message.data(), message.size(), 0);
        if (sendRes < 0)
        {
            std::cout << "Error while sending..." << std::endl;
            return false;
        }
        return true;

        // // closing the connected socket
        // close(m_client_fd);
    }

    bool readMessage(std::string &message)
    {
        char buffer[1024] = {0};
        int valread = read(m_client_fd, buffer, 1024);

        if (valread <= 0)
            return false;

        message.assign(buffer, buffer + valread);
        return true;
    }
};

int sendHttpRequest()
{

    // std::ifstream file(TEST_FILE);
    // if (!file.is_open())
    // {
    //     std::cout << "Failed to open test file" << std::endl;
    //     return -1;
    // }
    // std::string strBuffer((std::istreambuf_iterator<char>(file)),
    //                       std::istreambuf_iterator<char>());

    HttpRequest test(HttpRequestMethod::POST, "/data", "HTTP/1.0");

    test.setUserAgent("Mozilla/4.0 (compatible; MSIE5.01; Windows NT)");
    test.setHost("localhost::8080");
    test.setConnection("Keep-Alive");
    test.setBody("field1=value1&field2=value2", "application/x-www-form-urlencoded");
    test.createMessage();
    std::string strBuffer = test.getBuildedMessage();

    std::ofstream logFile("log_file.txt");
    logFile << strBuffer;
    logFile.close();

    ClientSocket socket(PORT);

    if (socket.connectServer())
    {
        if (!socket.sendMessage(strBuffer))
            return -1;

        std::string recvMsg;
        if (socket.readMessage(recvMsg))
        {
            // std::cout << "Recv Msg From Server: " << recvMsg << std::endl;
        }

        return 0;
    }

    return -1;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        std::cout << "Invalid Argument. Please provide number of client request..." << std::endl;
        return -1;
    }

    size_t numberOfClient = atoi(argv[1]);
    std::cout << "\n\n************* Sending " << numberOfClient << " Request Concurrently *************\n\n";
    // const size_t numberOfClient = 1000;
    std::vector<std::future<int>> requestFuture;
    for (size_t i = 0; i < numberOfClient; ++i)
    {
        requestFuture.push_back(std::async(std::launch::async, sendHttpRequest));
    }

    for (size_t i = 0; i < requestFuture.size(); ++i)
    {
        if (requestFuture[i].get() != 0)
        {
            std::cout << "Future #" << i + 1 << ": Failed\n";
        }
    }

    return 0;
}