#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <future>
#include <iostream>
#include <thread_safe_queue.hpp>
#include <http_request.hpp>
#include <http_response.hpp>
#include <thread>

constexpr size_t DEFAULT_THREAD_POOL_SIZE = 5;
constexpr size_t MAX_THREAD_POOL_SIZE = 8;
constexpr size_t MAX_BYTES_READ_PER_TIME = 30000;

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
    bool m_stop{false};
    char m_buffer[MAX_THREAD_POOL_SIZE][MAX_BYTES_READ_PER_TIME]{0};
    std::unique_ptr<ServerSocket> m_socket_;
    ThreadSafeQueue<ProcessingData> m_request_queue_;
    ThreadSafeQueue<int> m_incoming_socket_fd;

    std::future<int> m_handle_request_future;
    std::future<int> m_handle_connection_future;
    std::vector<std::thread> m_handle_connection_thread;
    std::map<std::string, std::function<HttpResponse()>> m_get_handle;

protected:
    /*
        When server start listening, will do folloing steps
        1. Spawn a certain number of thread will handle read the message from any connected socket (Here I use 4 thread)
        2. Start an async thread that will keep listen/accept and push new connection socket to the queue for 'handleConnectionThread' to process it (read the message)
        3. Start an async thread that will handle the request and send the response to the client
    */
    int handleListening();                        //(1) Async thread for accept new socket connection
    void handleConnectionThread(size_t threadId); //(2) Threads for reading the message from connected socket
    int handleRequestThread();                    //(3) Async thread for send response back to the client

    HttpResponse handleRequest(const std::shared_ptr<HttpRequest> &requestInfo);
    HttpResponse handleGetRequest(const std::shared_ptr<HttpRequest> &requestInfo);

public:
    HttpServer();
    ~HttpServer();
    bool listen(int portNumber, int backlog, size_t threadPoolSize = DEFAULT_THREAD_POOL_SIZE);
    bool readMessageFrom(int newSocket, size_t threadId);
    void GET(const std::string &resourceUri, std::function<HttpResponse()> handleRequest);
};