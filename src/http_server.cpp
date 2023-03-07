#include <http_server.hpp>
#include <server_utils.hpp>
#include <iostream>
#include <http_response.hpp>
#include <server_socket.hpp>

HttpServer::HttpServer()
{
}
HttpServer::~HttpServer()
{
}

bool HttpServer::listen(int portNumber, int backlog, size_t threadPoolSize)
{
    std::cout << "\n***** Server start listen on port=" << portNumber << ", backlog=" << backlog << ", thread_pool=" << threadPoolSize << std::endl
              << std::endl;

    m_socket_ = std::make_unique<ServerSocket>(portNumber, backlog);
    if (m_socket_ == nullptr)
    {
        std::cerr << "HttpServer::listen() Failed cause server socket is not initilized yet" << std::endl;
        return false;
    }

    for (size_t i = 0; i < threadPoolSize; ++i)
    {
        m_handle_connection_thread.push_back(std::thread(&HttpServer::handleConnectionThread, this, i));
    }
    m_handle_connection_future = std::async(std::launch::async, &HttpServer::handleListening, this);
    m_handle_request_future = std::async(std::launch::async, &HttpServer::handleRequestThread, this);
    for (size_t i = 0; i < threadPoolSize; ++i)
    {
        m_handle_connection_thread[i].join();
    }

    return true;
}

int HttpServer::handleListening()
{
    if (m_socket_ == nullptr)
    {
        std::cerr << "HttpServer::handleAcceptConnection() Failed cause server socket is not initilized yet" << std::endl;
        return -1;
    }
    if (!m_socket_->startListening())
    {
        std::cerr << "HttpServer::handleAcceptConnection() Failed cause server socket not started listening yet" << std::endl;
        return -1;
    }
    std::cout << "*********** WAITING FOR CONNECTIONSS ***********" << std::endl;

    while (!m_stop)
    {
        int newSocket;
        if ((newSocket = m_socket_->handleIncomingConnection()) < 0)
        {
            perror("accept");
            continue;
        }
        m_incoming_socket_fd.push(newSocket);
    }

    return 0;
}

void HttpServer::handleConnectionThread(size_t threadId)
{
    std::string threadIdStr = std::to_string(threadId);
    std::string debugMsg = "Spawn Thread ID=" + threadIdStr + " For Handle Incoming Connection\n";
    std::cout << debugMsg;
    while (!m_stop)
    {
        int socketFd = m_incoming_socket_fd.pop();
        readMessageFrom(socketFd, threadId);
    }
    std::cout << "Thread ID=" << threadId << " For Handle Incoming Connection Exiting." << std::endl;
}

bool HttpServer::readMessageFrom(int newSocket, size_t threadId)
{
    int bytesRead = m_socket_->readMessage(newSocket, m_buffer[threadId], MAX_BYTES_READ_PER_TIME);
    std::string bufferMsg;
    bufferMsg.assign(m_buffer[threadId], m_buffer[threadId] + bytesRead);

    HttpHeader recvHeader;
    size_t endHeaderPos = 0;
    ProcessingData data;
    data.socket_fd = newSocket;
    if (!http_utils::getHttpHeaderFrom(bufferMsg, recvHeader, endHeaderPos))
    {
        std::cerr << "Failed When Decode Http Header from Message" << std::endl;
        data.response_code = HttpStatusCode::BadRequest;
        m_request_queue_.push(data);
        return false;
    }
    std::string bodyMsg = bufferMsg.substr(endHeaderPos + HttpHeader::HTTP_END_LINE.size(), bufferMsg.size());
    if (bodyMsg.size() > 0 && !recvHeader.isHasContentLength())
    {
        // HTTP/1.1 requests containing a message-body MUST include a valid Content-Length header field unless the server is known to be HTTP/1.1 compliant
        data.response_code = HttpStatusCode::BadRequest;
        m_request_queue_.push(data);
        return false;
    }
    else if (bodyMsg.size() > 0 && recvHeader.isHasContentLength())
    {
        // Try to receive the rest of message
        if (bodyMsg.size() < recvHeader.getContentLength())
        {
            size_t bytesRemain = recvHeader.getContentLength() - bodyMsg.size();
            while (bytesRemain > 0)
            {
                bytesRead = m_socket_->readMessage(newSocket, m_buffer[threadId], bytesRemain);
                if (bytesRead < 0)
                {
                    std::cerr << "ERROR occur during read from socket\n";
                    data.response_code = HttpStatusCode::BadRequest;
                    m_request_queue_.push(data);
                    return false;
                }
                std::string readBuffer;
                readBuffer.assign(m_buffer[threadId], m_buffer[threadId] + bytesRead);
                bodyMsg.append(readBuffer);

                bytesRemain -= bytesRead;
            }
        }
    }

    if (recvHeader.getHttpVersion() != HTTP_VERSION_1_1)
    {
        data.response_code = HttpStatusCode::HTTPVersionNotSupported;
        m_request_queue_.push(data);
        return false;
    }

    data.response_code = HttpStatusCode::OK;
    data.request_ptr = std::make_shared<HttpRequest>(std::move(recvHeader), std::move(bodyMsg));
    m_request_queue_.push(data);
    return true;
}

int HttpServer::handleRequestThread()
{
    std::cout << "**** Handle Request Thread Started ****\n";

    while (!m_stop)
    {
        ProcessingData data = m_request_queue_.pop();
        HttpResponse response(data.response_code);

        if (data.response_code == HttpStatusCode::OK)
        {
            response = handleRequest(data.request_ptr);
        }

        response.createMessage();
        std::string responseMsg = response.getBuildedMessage();

        // std::cout << "!!!!!!!!!!!!Sending Http Response!!!!!!!!!!!!\n";
        m_socket_->sendMessage(data.socket_fd, responseMsg);
        // std::cout << "!!!!!!!!!!!!Sent Http Response!!!!!!!!!!!!\n\n";
        m_socket_->closeConnection(data.socket_fd);
    }
    std::cout << "Handle Request Thread Stopped" << std::endl;
    return 0;
}

HttpResponse HttpServer::handleRequest(const std::shared_ptr<HttpRequest> &requestInfo)
{
    assert(requestInfo != nullptr);

    HttpResponse response(HttpStatusCode::NotImplemented);

    switch (requestInfo->getMethod())
    {
    case HttpRequestMethod::GET:
        return handleGetRequest(requestInfo);
        break;
    default:
        break;
    }
    return response;
}

HttpResponse HttpServer::handleGetRequest(const std::shared_ptr<HttpRequest> &requestInfo)
{
    std::string requestUri = requestInfo->getRequestUri();
    if (m_get_handle.find(requestUri) == m_get_handle.end())
    {
        return HttpResponse(HttpStatusCode::NotFound);
    }
    return m_get_handle[requestUri]();
}

void HttpServer::GET(const std::string &resourceUri, std::function<HttpResponse()> handleRequest)
{
    std::string debugMsg = "Add GET Resource [" + resourceUri + "]\n";
    std::cout << debugMsg;
    m_get_handle.insert(std::pair<std::string, std::function<HttpResponse()>>(resourceUri, handleRequest));
}
