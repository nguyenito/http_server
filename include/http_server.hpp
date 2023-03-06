#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <future>
#include <iostream>
#include <thread_safe_queue.hpp>
#include <http_request.hpp>
#include <http_response.hpp>

constexpr size_t MAX_BYTES_READ_PER_TIME = 30000;
constexpr size_t CONNECTION_TIMEOUT_SECONDS = 5;

struct ProcessingData
{
    std::shared_ptr<HttpRequest> request_ptr;
    int socket_fd;
    HttpStatusCode response_code;
};

class ServerSocket;
class HttpServer
{
private:
    char m_buffer[MAX_BYTES_READ_PER_TIME]{0};
    std::unique_ptr<ServerSocket> m_socket_;
    ThreadSafeQueue<ProcessingData> m_request_queue_;
    bool m_stop{false};
    std::future<int> m_handle_request_future;
    std::future<int> m_handle_connection_future;

    size_t m_connection_count{0};

public:
    HttpServer();
    ~HttpServer();
    bool listen(int portNumber, int backlog);

    int readMessage(int newSocket);
    int handleAcceptConnection();
    int handleRequestThread();
    void handleRespondThread();
};