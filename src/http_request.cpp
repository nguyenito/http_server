#include <http_request.hpp>
#include <server_utils.hpp>

std::unordered_map<HttpRequestMethod, std::string> HttpRequest::REQUEST_METHOD_TO_NAME = {
    {HttpRequestMethod::GET, "GET"},
    {HttpRequestMethod::POST, "POST"},
    {HttpRequestMethod::PUT, "PUT"},
    {HttpRequestMethod::DELETE, "DELETE"},
    {HttpRequestMethod::PATCH, "PATCH"},
    {HttpRequestMethod::HEAD, "HEAD"},
    {HttpRequestMethod::CONNECT, "CONNECT"},
    {HttpRequestMethod::OPTIONS, "OPTIONS"},
    {HttpRequestMethod::TRACE, "TRACE"},
    {HttpRequestMethod::UNKNOWN, "UNKNOWN"},
};

std::unordered_map<std::string, HttpRequestMethod> HttpRequest::NAME_TO_REQUEST_METHOD = {
    {"GET", HttpRequestMethod::GET},
    {"POST", HttpRequestMethod::POST},
    {"PUT", HttpRequestMethod::PUT},
    {"DELETE", HttpRequestMethod::DELETE},
    {"PATCH", HttpRequestMethod::PATCH},
    {"HEAD", HttpRequestMethod::HEAD},
    {"CONNECT", HttpRequestMethod::CONNECT},
    {"OPTIONS", HttpRequestMethod::OPTIONS},
    {"TRACE", HttpRequestMethod::TRACE},
};

HttpRequest::HttpRequest(const HttpRequestMethod &method, const std::string &uri, const std::string &httpVersion) : m_method(method), m_request_uri(uri)
{
    m_header.setHttpVersion(httpVersion);
}

HttpRequest::HttpRequest(const HttpHeader &header, const std::string &body, const std::string &httpVersion) : HttpMessage(header, body, httpVersion)
{
    _decodeRequest();
}

HttpRequest::HttpRequest(HttpHeader &&header, std::string &&body) : HttpMessage(std::move(header), std::move(body))
{
    _decodeRequest();
}

bool HttpRequest::_decodeRequest()
{
    std::string methodName, requestUri, version;
    if (!http_utils::getHttpRequestLineFrom(m_header.getStartLine(), methodName, requestUri, version))
        return false;

    if (NAME_TO_REQUEST_METHOD.find(methodName) != NAME_TO_REQUEST_METHOD.end())
    {
        setMethod(NAME_TO_REQUEST_METHOD[methodName]);
    }
    else
    {
        setMethod(HttpRequestMethod::UNKNOWN);
    }
    setRequestUri(requestUri);
    m_header.setHttpVersion(std::move(version));

    return true;
}

bool HttpRequest::createRequest(const HttpHeader &header, const std::string &body)
{
    m_header = header;
    m_body = body;
    return _decodeRequest();
}

bool HttpRequest::createRequest(HttpHeader &&header, std::string &&body)
{
    m_header = std::move(header);
    m_body = std::move(body);
    return _decodeRequest();
}

std::string HttpRequest::getMethodName() const
{
    return REQUEST_METHOD_TO_NAME[m_method];
}

HttpRequestMethod HttpRequest::getMethod() const
{
    return m_method;
}

void HttpRequest::setMethod(const HttpRequestMethod &method)
{
    m_method = method;
}

void HttpRequest::setRequestUri(const std::string &uri)
{
    m_request_uri = uri;
}

std::string HttpRequest::getRequestUri() const
{
    return m_request_uri;
}

void HttpRequest::buildStartLine()
{
    std::string startLine = REQUEST_METHOD_TO_NAME[m_method] + HttpHeader::HTTP_INITIAL_LINE_SEPARATOR + m_request_uri + HttpHeader::HTTP_INITIAL_LINE_SEPARATOR + m_header.getHttpVersion();
    m_header.setStartLine(startLine);
}

void HttpRequest::printHeader() const
{
    m_header.print();
}