#pragma once
#include <string>
#include <iostream>
#include <http_header.hpp>

class HttpMessage
{
private:
    std::string m_builded_message;

protected:
    std::string m_body;
    HttpHeader m_header;

    void setContentType(const std::string &value);
    void setContentLength(size_t contentLength);

    virtual void buildStartLine() = 0; // Its depend to http request (request line) or http response(status line)
    void buildMessage();               // Build the start-line, header then the body

public:
    HttpMessage() = default;
    virtual ~HttpMessage() = default;

    HttpMessage(const HttpMessage &other);
    HttpMessage(HttpMessage &&other);
    HttpMessage(const HttpHeader &header, const std::string &body, const std::string &httpVersion = HTTP_VERSION_1_1);
    HttpMessage(HttpHeader &&header, std::string &&body, const std::string &httpVersion = HTTP_VERSION_1_1);

    HttpMessage &operator=(const HttpMessage &other);
    HttpMessage &operator=(HttpMessage &&other);

    void setServer(const std::string &value);
    void setFrom(const std::string &value);
    void setHost(const std::string &value);
    void setUserAgent(const std::string &value);
    void setConnection(const std::string &value);
    void setDate(const std::string &value);
    void setBody(const std::string &body, const std::string &contentType);
    void setBody(const std::string &&body, const std::string &contentType);
    std::string getBody() const;

    std::string getBuildedMessage() const;
    virtual void createMessage();
};