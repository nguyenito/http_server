#include <http_server.hpp>
#include <server_utils.hpp>
#include <iostream>
#include <http_response.hpp>
#include <server_socket.hpp>
// #include <unistd.h>

HttpServer::HttpServer()
{
}
HttpServer::~HttpServer()
{
}

bool HttpServer::listen(int portNumber, int backlog)
{
    m_socket_ = std::make_unique<ServerSocket>(portNumber, backlog);
    if (m_socket_ == nullptr)
    {
        std::cerr << "HttpServer::listen() Failed cause server socket is not initilized yet" << std::endl;
        return false;
    }
    m_handle_connection_future = std::async(std::launch::async, &HttpServer::handleAcceptConnection, this);
    m_handle_request_future = std::async(std::launch::async, &HttpServer::handleRequestThread, this);

    return true;
}

int HttpServer::handleAcceptConnection()
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

    while (!m_stop)
    {

        std::cout << "*********** WAITING FOR NEW CONNECTION ***********" << std::endl;
        std::cout << "*********** Connection " << m_connection_count << " count ***********" << std::endl;

        int newSocket;
        if ((newSocket = m_socket_->acceptConnection()) < 0)
        {
            perror("accept");
            // sleep(1);
            continue;
        }
        ++m_connection_count;
        readMessage(newSocket);

        // m_socket_->closeConnection(newSocket);

        std::cout << std::endl;
    }
    return 0;
}

int HttpServer::readMessage(int newSocket)
{
    int bytesRead = m_socket_->readMessage(newSocket, m_buffer, MAX_BYTES_READ_PER_TIME);
    std::string bufferMsg;
    bufferMsg.assign(m_buffer, m_buffer + bytesRead);

    HttpHeader recvHeader;
    size_t endHeaderPos = 0;
    ProcessingData data;
    data.socket_fd = newSocket;
    if (!http_utils::getHttpHeaderFrom(bufferMsg, recvHeader, endHeaderPos))
    {
        std::cerr << "Failed When Decode Http Header from Message" << std::endl;
        data.response_code = HttpStatusCode::BadRequest;
        m_request_queue_.push(data);
        return bytesRead;
    }
    std::string bodyMsg = bufferMsg.substr(endHeaderPos + HttpHeader::HTTP_END_LINE.size(), bufferMsg.size());
    if (bodyMsg.size() > 0 && !recvHeader.isHasContentLength())
    {
        // HTTP/1.1 requests containing a message-body MUST include a valid Content-Length header field unless the server is known to be HTTP/1.1 compliant
        data.response_code = HttpStatusCode::BadRequest;
        m_request_queue_.push(data);
        return bytesRead;
    }

    // if (recvHeader.getHttpVersion() != HTTP_VERSION_1_1)
    // {
    //     data.response_code = HttpStatusCode::HTTPVersionNotSupported;
    //     m_request_queue_.push(data);
    //     return bytesRead;
    // }

    data.response_code = HttpStatusCode::OK;
    data.request_ptr = std::make_shared<HttpRequest>(std::move(recvHeader), std::move(bodyMsg));
    m_request_queue_.push(data);
    return bytesRead;
}

int HttpServer::handleRequestThread()
{
    std::cout << "Handle Request Thread Started" << std::endl;

    const std::string startHtmlBody = "<html>\r\n<body><h1>";
    const std::string endHtmlBody = "</h1></body></html>";
    while (!m_stop)
    {
        ProcessingData data = m_request_queue_.pop();
        if (data.response_code == HttpStatusCode::OK)
        {
            HttpResponse response(HttpStatusCode::OK);
            response.setServer("HttpCppSimple");
            std::string htmlBody = "Hello From HttpCpp Server<br>Reply to " + data.request_ptr->getMethodName() + " Http Method";
            htmlBody = startHtmlBody + htmlBody + endHtmlBody;
            response.setBody(htmlBody, "text/html");
            response.createMessage();
            std::string responseMsg = response.getBuildedMessage();

            std::cout << "!!!!!!!!!!!!Sending Http Response!!!!!!!!!!!!" << std::endl;
            m_socket_->sendMessage(data.socket_fd, responseMsg);
            std::cout << "!!!!!!!!!!!!Sent Http Response!!!!!!!!!!!!" << std::endl;

            // if (data.request_ptr->getMethod() == HttpRequestMethod::POST)
            // {
            //     std::cout << "+++++++++++++++++++ Http Body +++++++++++++++++++" << std::endl;
            //     std::cout << data.request_ptr->getBody() << std::endl;
            //     std::cout << "+++++++++++++++++++ End Body+++++++++++++++++++" << std::endl;
            // }
        }
        else
        {
            HttpResponse response(data.response_code);
            response.createMessage();
            std::string responseMsg = response.getBuildedMessage();

            std::cout << "!!!!!!!!!!!!Sending Http Response Error!!!!!!!!!!!!" << std::endl;
            m_socket_->sendMessage(data.socket_fd, responseMsg);
            std::cout << responseMsg << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        }

        // m_socket_->closeConnection(data.socket_fd);
    }
    std::cout << "Handle Request Thread Stopped" << std::endl;
    return 0;
}

void HttpServer::handleRespondThread()
{
}