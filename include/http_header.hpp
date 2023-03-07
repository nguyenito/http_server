#pragma once
#include <string>
#include <map>
#include <unordered_map>

const std::string HTTP_VERSION_1_1 = "HTTP/1.1";

enum class HeaderAttribute
{
    ContentType,
    ContentLength,
    Host,
    UserAgent,
    Connection,
    Date,
    LastModified,
    Server,
    From
};

class HttpHeader
{
protected:
    std::string m_start_line;
    std::string m_http_version{HTTP_VERSION_1_1};
    std::map<std::string, std::string> m_attributes;

    void add(const std::string &key, const std::string &value);
    void add(const std::string &key, std::string &&value);

    friend class HttpMessage;

public:
    static std::unordered_map<HeaderAttribute, std::string> HeaderAttributeName;
    static std::string HTTP_END_LINE;
    static std::string HTTP_INITIAL_LINE_SEPARATOR;

    HttpHeader() = default;
    HttpHeader(const HttpHeader &other);
    HttpHeader(HttpHeader &&other);
    HttpHeader &operator=(const HttpHeader &other);
    HttpHeader &operator=(HttpHeader &&other);

    bool isHas(const std::string &fieldName) const;
    bool isHasContentLength() const;
    size_t getContentLength() const;
    std::string getStartLine() const;
    void print() const;
    void addHeader(const std::string &key, const std::string &value);
    void setStartLine(const std::string &value);
    std::string getHttpVersion() const;
    void setHttpVersion(const std::string &version);
    void setHttpVersion(std::string &&version);
};
