#pragma once
#include <string>
#include <iostream>
#include <http_message.hpp>

enum class HttpRequestMethod
{
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
    UNKNOWN

};
class HttpRequest : public HttpMessage
{
private:
    std::string m_request_uri;
    HttpRequestMethod m_method{HttpRequestMethod::UNKNOWN};

    bool _decodeRequest(); // decode it's request information based on its current header information
    void buildStartLine() override;

public:
    static std::unordered_map<HttpRequestMethod, std::string> REQUEST_METHOD_TO_NAME;
    static std::unordered_map<std::string, HttpRequestMethod> NAME_TO_REQUEST_METHOD;

    HttpRequest(const HttpRequestMethod &method, const std::string &uri, const std::string &httpVersion = HTTP_VERSION_1_1);
    HttpRequest(const HttpHeader &header, const std::string &body, const std::string &httpVersion = HTTP_VERSION_1_1);
    HttpRequest(HttpHeader &&header, std::string &&body);

    bool createRequest(const HttpHeader &header, const std::string &body);
    bool createRequest(HttpHeader &&header, std::string &&body);

    void setMethod(const HttpRequestMethod &method);
    HttpRequestMethod getMethod() const;
    std::string getMethodName() const;

    void setRequestUri(const std::string &uri);
    std::string getRequestUri() const;

    void printHeader() const;
};
