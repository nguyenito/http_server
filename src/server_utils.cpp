#include <server_utils.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>

namespace http_utils
{
    std::vector<std::string> splitString(const std::string &data, const std::string &delimeter, const bool &ignoreEmptyString)
    {
        std::vector<std::string> tokens;
        std::string::size_type pos = 0, prev = 0;
        std::string token;
        while ((pos = data.find(delimeter, prev)) != std::string::npos)
        {
            token = data.substr(prev, pos - prev);
            if (!ignoreEmptyString || (ignoreEmptyString && token != ""))
            {
                tokens.push_back(token);
            }

            prev = pos + delimeter.size();
        }
        token = data.substr(prev);
        if (!ignoreEmptyString || (ignoreEmptyString && token != ""))
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool getHeaderField(const std::string &bufferMsg, std::string &fieldName, std::string &content)
    {
        std::string fieldMsg = "";

        std::string delimeter = ":";
        int pos = bufferMsg.find(delimeter, 0);
        if (pos == -1)
            return false;

        fieldName = bufferMsg.substr(0, pos);

        delimeter = " ";
        pos = bufferMsg.find(delimeter, pos);
        if (pos == -1)
            return false;

        content = bufferMsg.substr(pos + 1, bufferMsg.size());

        return true;
    }

    bool getHttpHeaderFrom(const std::string &bufferMsg, HttpHeader &parsedHeader, size_t &endHeaderPoistion)
    {
        std::string delimeter = HttpHeader::HTTP_END_LINE;
        int start, end = -1 * delimeter.size();

        // 1. Get start-line: Request-Line | Status-Line
        // Certain buggy HTTP/1.0 client implementations generate extra CRLF's after a POST request.
        // We should ignore any empty line(s) received where a Request-Line is expected.
        std::string startLineContent = "";
        while (startLineContent == "")
        {
            // 1. Decode start-line message
            start = end + delimeter.size();
            end = bufferMsg.find(delimeter, start);
            if (end == -1)
                return false;
            startLineContent = bufferMsg.substr(start, end - start);
        }
        parsedHeader.setStartLine(startLineContent);

        // 2. Get headers field
        do
        {
            start = end + delimeter.size();
            end = bufferMsg.find(delimeter, start);
            if (end == -1)
                break;

            std::string lineContent = bufferMsg.substr(start, end - start);
            // Empty Line ==> End of Header File
            if (lineContent == "")
            {
                endHeaderPoistion = end;
                break;
            }

            else
            {
                std::string fieldName, content;
                if (!getHeaderField(lineContent, fieldName, content))
                {
                    std::cerr << "Invalid Header Content: " << lineContent << std::endl;
                    return false;
                }

                parsedHeader.addHeader(fieldName, content);
            }
        } while (end != -1);

        return true;
    }

    bool getHttpRequestLineFrom(const std::string &startLine, std::string &method, std::string &requestUri, std::string &version)
    {
        std::vector<std::string> parsedInfo = splitString(startLine, HttpHeader::HTTP_INITIAL_LINE_SEPARATOR, true);
        if (parsedInfo.size() < REQUEST_INFO_COUNT)
            return false;

        method = std::move(parsedInfo[RequestLineInfo::METHOD_NAME]);
        requestUri = std::move(parsedInfo[RequestLineInfo::REQUEST_URI]);
        version = std::move(parsedInfo[RequestLineInfo::VERSION]);

        return true;
    }

}
