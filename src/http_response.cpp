#include <http_response.hpp>

std::unordered_map<HttpStatusCode, std::string> HttpResponse::REASON_PHRASE = {
    // ####### 1xx - Informational #######
    {HttpStatusCode::Continue, "Continue"},
    {HttpStatusCode::SwitchingProtocols, "Switching Protocols"},
    {HttpStatusCode::Processing, "Processing"},
    {HttpStatusCode::EarlyHints, "Early Hints"},
    // ####### 2xx - Successful #######
    {HttpStatusCode::OK, "OK"},
    {HttpStatusCode::Created, "Created"},
    {HttpStatusCode::Accepted, "Accepted"},
    {HttpStatusCode::NonAuthoritativeInformation, "Non-Authoritative Information"},
    {HttpStatusCode::NoContent, "NoContent"},
    {HttpStatusCode::ResetContent, "ResetContent"},
    {HttpStatusCode::PartialContent, "PartialContent"},
    {HttpStatusCode::MultiStatus, "MultiStatus"},
    {HttpStatusCode::AlreadyReported, "AlreadyReported"},
    {HttpStatusCode::IMUsed, "IMUsed"},
    // ####### 3xx - Redirection #######
    {HttpStatusCode::MultipleChoices, "MultipleChoices"},
    {HttpStatusCode::MovedPermanently, "MovedPermanently"},
    {HttpStatusCode::Found, "Found"},
    {HttpStatusCode::SeeOther, "SeeOther"},
    {HttpStatusCode::NotModified, "NotModified"},
    {HttpStatusCode::TemporaryRedirect, "TemporaryRedirect"},
    {HttpStatusCode::PermanentRedirect, "PermanentRedirect"},
    // ####### 4xx - Client Error #######
    {HttpStatusCode::BadRequest, "BadRequest"},
    {HttpStatusCode::Unauthorized, "Unauthorized"},
    {HttpStatusCode::PaymentRequired, "PaymentRequired"},
    {HttpStatusCode::Forbidden, "Forbidden"},
    {HttpStatusCode::NotFound, "NotFound"},
    {HttpStatusCode::MethodNotAllowed, "MethodNotAllowed"},
    {HttpStatusCode::NotAcceptable, "NotAcceptable"},
    {HttpStatusCode::ProxyAuthenticationRequired, "ProxyAuthenticationRequired"},
    {HttpStatusCode::RequestTimeout, "RequestTimeout"},
    {HttpStatusCode::Conflict, "Conflict"},
    {HttpStatusCode::Gone, "Gone"},
    {HttpStatusCode::LengthRequired, "LengthRequired"},
    {HttpStatusCode::PreconditionFailed, "PreconditionFailed"},
    {HttpStatusCode::ContentTooLarge, "ContentTooLarge"},
    {HttpStatusCode::URITooLong, "URITooLong"},
    {HttpStatusCode::UnsupportedMediaType, "UnsupportedMediaType"},
    {HttpStatusCode::RangeNotSatisfiable, "RangeNotSatisfiable"},
    {HttpStatusCode::ExpectationFailed, "ExpectationFailed"},
    {HttpStatusCode::ImATeapot, "ImATeapot"},
    {HttpStatusCode::MisdirectedRequest, "MisdirectedRequest"},
    {HttpStatusCode::UnprocessableContent, "UnprocessableContent"},
    {HttpStatusCode::Locked, "Locked"},
    {HttpStatusCode::FailedDependency, "FailedDependency"},
    {HttpStatusCode::TooEarly, "TooEarly"},
    {HttpStatusCode::UpgradeRequired, "UpgradeRequired"},
    {HttpStatusCode::PreconditionRequired, "PreconditionRequired"},
    {HttpStatusCode::TooManyRequests, "TooManyRequests"},
    {HttpStatusCode::RequestHeaderFieldsTooLarge, "RequestHeaderFieldsTooLarge"},
    {HttpStatusCode::UnavailableForLegalReasons, "UnavailableForLegalReasons"},
    // ####### 5xx - Server Error #######
    {HttpStatusCode::InternalServerError, "Internal Server Error"},
    {HttpStatusCode::NotImplemented, "Not Implemented"},
    {HttpStatusCode::BadGateway, "Bad Gateway"},
    {HttpStatusCode::ServiceUnavailable, "Service Unavailable"},
    {HttpStatusCode::GatewayTimeout, "Gateway Timeout"},
    {HttpStatusCode::HTTPVersionNotSupported, "HTTP Version Not Supported"},
    {HttpStatusCode::VariantAlsoNegotiates, "Variant Also Negotiates"},
    {HttpStatusCode::InsufficientStorage, "Insufficient Storage"},
    {HttpStatusCode::LoopDetected, "Loop Detected"},
    {HttpStatusCode::NotExtended, "Not Extended"},
    {HttpStatusCode::NetworkAuthenticationRequired, "Network Authentication Required"},
};

std::string HttpResponse::GetReasonPhrase(const HttpStatusCode &statusCode)
{
    if (REASON_PHRASE.find(statusCode) != REASON_PHRASE.end())
        return REASON_PHRASE[statusCode];

    return "Unknown Reason";
}

HttpResponse::HttpResponse(const HttpStatusCode &statusCode) : m_status(statusCode)
{
}

inline std::string HttpResponse::getStatusCode() const
{
    return std::to_string(static_cast<int>(m_status));
}

void HttpResponse::buildStartLine()
{
    std::string startLine = m_header.getHttpVersion() + HttpHeader::HTTP_INITIAL_LINE_SEPARATOR + getStatusCode() + HttpHeader::HTTP_INITIAL_LINE_SEPARATOR + GetReasonPhrase(m_status);
    m_header.setStartLine(startLine);
}
