#include <http_server.hpp>
#include <http_response.hpp>
#include <fstream>
#include <ini_reader.h>

#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 100

void readConfig(int &portNumber, int &backlog, size_t &threadPoolSize)
{
    portNumber = DEFAULT_PORT;
    backlog = DEFAULT_BACKLOG;
    threadPoolSize = DEFAULT_THREAD_POOL_SIZE;

    mINI::INIFile file("../config/server_setup.ini");
    mINI::INIStructure configIni;
    bool readSuccess = file.read(configIni);
    if (!readSuccess)
    {
        std::cerr << "Fail to load server_setup.ini file" << std::endl;
        return;
    }

    if (configIni["server"]["port"] != "")
    {
        portNumber = std::stoi(configIni["server"]["port"]);
        if (portNumber <= 0)
        {
            portNumber = DEFAULT_PORT;
        }
    }
    configIni["server"]["port"] = std::to_string(portNumber);

    if (configIni["server"]["backlog"] != "")
    {
        backlog = std::stoi(configIni["server"]["backlog"]);
        if (backlog <= 0)
        {
            backlog = DEFAULT_BACKLOG;
        }
    }
    configIni["server"]["backlog"] = std::to_string(backlog);

    if (configIni["server"]["thread_pool_size"] != "")
    {
        int poolSize = std::stoi(configIni["server"]["thread_pool_size"]);
        if (poolSize > 0)
        {
            threadPoolSize = static_cast<size_t>(poolSize);
        }
        if (threadPoolSize > MAX_THREAD_POOL_SIZE)
            threadPoolSize = MAX_THREAD_POOL_SIZE;
    }
    configIni["server"]["thread_pool_size"] = std::to_string(threadPoolSize);

    file.write(configIni);
}

int main()
{

    int port, backlog;
    size_t threadPoolSize;
    readConfig(port, backlog, threadPoolSize);

    std::vector<std::string> simpleWebpageHtml;
    size_t index = 1;
    while (true)
    {
        std::string SIMPLE_WEBPAGE_PATH = "../test/simple_webpage_" + std::to_string(index) + ".html";
        std::ifstream file(SIMPLE_WEBPAGE_PATH);
        if (!file.is_open())
            break;

        simpleWebpageHtml.push_back(std::string((std::istreambuf_iterator<char>(file)),
                                                std::istreambuf_iterator<char>()));
        file.close();
        ++index;
    }

    HttpServer server;
    for (size_t i = 0; i < simpleWebpageHtml.size(); ++i)
    {
        std::string resourceUri = "/" + std::to_string(i + 1);
        server.GET(resourceUri, [&]()
                   {
                   HttpResponse homepageRes(HttpStatusCode::OK);

                   homepageRes.setBody(simpleWebpageHtml[i], "text/html");
                   return homepageRes; });
    }
    server.listen(port, backlog, threadPoolSize);
    return 0;
}