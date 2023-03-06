#include <http_message.hpp>
#include <algorithm>
#include <assert.h>

HttpMessage::HttpMessage(const HttpMessage &other) : m_body(other.m_body), m_header(other.m_header)
{
}

HttpMessage::HttpMessage(HttpMessage &&other) : m_body(std::move(other.m_body)), m_header(std::move(other.m_header))
{
}

HttpMessage::HttpMessage(const HttpHeader &header, const std::string &body, const std::string &httpVersion) : m_header(header), m_body(body)
{
    m_header.setHttpVersion(httpVersion);
}

HttpMessage::HttpMessage(HttpHeader &&header, std::string &&body, const std::string &httpVersion) : m_header(std::move(header)), m_body(std::move(body))
{
    m_header.setHttpVersion(std::move(httpVersion));
}

HttpMessage &HttpMessage::operator=(const HttpMessage &other)
{
    if (&other != this)
    {
        m_body = other.m_body;
        m_header = other.m_header;
    }
    return *this;
}

HttpMessage &HttpMessage::operator=(HttpMessage &&other)
{
    if (&other != this)
    {
        m_body = std::move(other.m_body);
        m_header = std::move(other.m_header);
    }
    return *this;
}

void HttpMessage::setContentType(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::ContentType], value);
}

void HttpMessage::setContentLength(size_t contentLength)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::ContentLength], std::to_string(contentLength));
}

void HttpMessage::setServer(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::Server], value);
}

void HttpMessage::setFrom(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::From], value);
}

void HttpMessage::setHost(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::Host], value);
}

void HttpMessage::setUserAgent(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::UserAgent], value);
}

void HttpMessage::setConnection(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::Connection], value);
}

void HttpMessage::setDate(const std::string &value)
{
    m_header.add(HttpHeader::HeaderAttributeName[HeaderAttribute::Date], value);
}

void HttpMessage::setBody(const std::string &body, const std::string &contentType)
{
    assert(body.size() > 0);

    m_body = body;
    setContentType(contentType);
    setContentLength(m_body.size());
}

void HttpMessage::setBody(const std::string &&body, const std::string &contentType)
{
    assert(body.size() > 0);

    m_body = std::move(body);
    setContentType(contentType);
    setContentLength(m_body.size());
}

std::string HttpMessage::getBody() const
{
    return m_body;
}

void HttpMessage::buildMessage()
{
    m_builded_message.clear();
    // Build start-line
    m_builded_message = m_header.m_start_line + HttpHeader::HTTP_END_LINE;

    // Build header
    for (const auto &it : m_header.m_attributes)
    {
        m_builded_message += it.first + ": " + it.second + HttpHeader::HTTP_END_LINE;
    }

    // Build body if any
    if (m_body != "")
    {
        m_builded_message += HttpHeader::HTTP_END_LINE + m_body;
    }
}

std::string HttpMessage::getBuildedMessage() const
{
    return m_builded_message;
}

void HttpMessage::createMessage()
{
    buildStartLine();
    buildMessage();
}
