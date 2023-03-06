#pragma once
#include <string>
#include <http_header.hpp>

namespace http_utils
{
    constexpr size_t REQUEST_INFO_COUNT = 3;
    enum RequestLineInfo
    {
        METHOD_NAME = 0,
        REQUEST_URI = 1,
        VERSION = 2,
    };
    std::vector<std::string> splitString(const std::string &data, const std::string &delimeter, const bool &ignoreEmptyString = false);

    bool getHttpHeaderFrom(const std::string &bufferMsg, HttpHeader &parsedHeader, size_t &endHeaderPoistion);
    bool getHttpRequestLineFrom(const std::string &startLine, std::string &method, std::string &requestUri, std::string &version);

}
