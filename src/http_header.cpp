#include <http_header.hpp>
#include <algorithm>
#include <iostream>

std::string HttpHeader::HTTP_END_LINE = "\r\n";
std::string HttpHeader::HTTP_INITIAL_LINE_SEPARATOR = " ";

std::unordered_map<HeaderAttribute, std::string> HttpHeader::HeaderAttributeName = {
    {HeaderAttribute::ContentType, "content-type"},
    {HeaderAttribute::ContentLength, "content-length"},
    {HeaderAttribute::Host, "host"},
    {HeaderAttribute::UserAgent, "user-agent"},
    {HeaderAttribute::Connection, "connection"},
    {HeaderAttribute::Date, "date"},
    {HeaderAttribute::LastModified, "last-modified"},
    {HeaderAttribute::Server, "server"},
    {HeaderAttribute::From, "from"},

};

HttpHeader::HttpHeader(const HttpHeader &other) : m_start_line(other.m_start_line), m_http_version(other.m_http_version), m_attributes(other.m_attributes)
{
}

HttpHeader::HttpHeader(HttpHeader &&other) : m_start_line(std::move(other.m_start_line)), m_http_version(std::move(other.m_http_version)), m_attributes(std::move(other.m_attributes))
{
}

HttpHeader &HttpHeader::operator=(const HttpHeader &other)
{
    if (&other != this)
    {
        m_attributes = other.m_attributes;
    }
    return *this;
}

HttpHeader &HttpHeader::operator=(HttpHeader &&other)
{
    if (&other != this)
    {
        m_attributes = std::move(other.m_attributes);
    }
    return *this;
}

bool HttpHeader::isHas(const std::string &fieldName) const
{
    // Since the header name in http protocol is not case sensitive hence we converted all our keys to lowercase
    std::string lowerCaseKey = fieldName;
    std::transform(lowerCaseKey.begin(), lowerCaseKey.end(), lowerCaseKey.begin(), [](char c)
                   { return std::tolower(c); });

    if (m_attributes.find(lowerCaseKey) != m_attributes.end())
        return true;
    else
        return false;
}

bool HttpHeader::isHasContentLength() const
{
    if (m_attributes.find(HeaderAttributeName[HeaderAttribute::ContentLength]) != m_attributes.end())
        return true;
    else
        return false;
}

size_t HttpHeader::getContentLength() const
{
    if (!isHasContentLength())
        return 0;

    return std::stoi(m_attributes.at(HeaderAttributeName[HeaderAttribute::ContentLength]));
}

std::string HttpHeader::getStartLine() const
{
    return m_start_line;
}

void HttpHeader::setStartLine(const std::string &value)
{
    m_start_line = value;
}

void HttpHeader::print() const
{
    std::cout << m_start_line << HttpHeader::HTTP_END_LINE;
    for (const auto &it : m_attributes)
    {
        std::cout << it.first << ": " << it.second << HttpHeader::HTTP_END_LINE;
    }
}

void HttpHeader::addHeader(const std::string &key, const std::string &value)
{
    // Since the header name in http protocol is not case sensitive hence we converted all our keys to lowercase
    std::string lowerCaseKey = key;
    std::transform(lowerCaseKey.begin(), lowerCaseKey.end(), lowerCaseKey.begin(), [](char c)
                   { return std::tolower(c); });

    this->add(lowerCaseKey, value);
}

void HttpHeader::add(const std::string &key, const std::string &value)
{
    if (m_attributes.find(key) == m_attributes.end())
    {
        m_attributes[key] = value;
    }
    else
    {
        m_attributes[key].append(", ");
        m_attributes[key].append(value);
    }
}

void HttpHeader::add(const std::string &key, std::string &&value)
{
    if (m_attributes.find(key) == m_attributes.end())
    {
        m_attributes[key] = std::move(value);
    }
    else
    {
        m_attributes[key].append(", ");
        m_attributes[key].append(std::move(value));
    }
}

std::string HttpHeader::getHttpVersion() const
{
    return m_http_version;
}

void HttpHeader::setHttpVersion(const std::string &version)
{
    m_http_version = version;
}

void HttpHeader::setHttpVersion(std::string &&version)
{
    m_http_version = std::move(version);
}
