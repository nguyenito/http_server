#include <http_server.hpp>

#define PORT 8080
constexpr int MAXIMUM_CONNECTION = 100;

int main()
{
    HttpServer server;
    server.listen(PORT, MAXIMUM_CONNECTION);

    return 0;
}