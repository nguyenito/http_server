#include <http_server.hpp>
#include <http_response.hpp>
#include <fstream>
#include <ini_reader.h>
#include <unistd.h>
#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 100

const std::string DEFAULT_HOMEPAGE_HTML = "<HTML><HEAD><TITLE>Albert Einstein Quotes</TITLE></HEAD><H1>Logic will get you from A to B. Imagination will take you everywhere</H1></HTML>";

void readConfig(int &portNumber, int &backlog, size_t &threadPoolSize, size_t &noOfWebpage)
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

    if (configIni["server"]["number_of_webpage"] != "")
    {
        int poolSize = std::stoi(configIni["server"]["number_of_webpage"]);
        if (poolSize > 0)
        {
            noOfWebpage = static_cast<size_t>(poolSize);
        }
    }
    configIni["server"]["number_of_webpage"] = std::to_string(noOfWebpage);

    file.write(configIni);
}

std::string readHtmlFromFile(const std::string &filePath)
{
    std::string fileContent = "";
    std::ifstream file(filePath);
    if (file.is_open())
    {
        fileContent = std::string((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        file.close();
    }

    return fileContent;
}

int main()
{

    int port, backlog;
    size_t threadPoolSize, noOfWebpage;
    readConfig(port, backlog, threadPoolSize, noOfWebpage);

    HttpServer server;
    // Get Home Page Request
    std::string HOME_WEBPAGE_PATH = "../test/simple_webpage_home.html";
    std::string homePageBody = readHtmlFromFile(HOME_WEBPAGE_PATH);
    if (homePageBody == "")
    {
        homePageBody = DEFAULT_HOMEPAGE_HTML;
    }
    server.GET("/", [&homePageBody]()
               {
                    HttpResponse homepageRes(HttpStatusCode::OK);
                   homepageRes.setBody(homePageBody, "text/html");
                   return homepageRes; });

    for (size_t pageIndex = 0; pageIndex < noOfWebpage; ++pageIndex)
    {
        std::string resourceUri = "/" + std::to_string(pageIndex + 1);
        server.GET(resourceUri, [pageIndex]()
                   {
                    HttpResponse homepageRes(HttpStatusCode::OK);

                    std::string SIMPLE_WEBPAGE_PATH = "../test/simple_webpage_" + std::to_string(pageIndex+1) + ".html";
                    std::string bodyContent  = readHtmlFromFile(SIMPLE_WEBPAGE_PATH);
                    if(bodyContent == "")
                        bodyContent = DEFAULT_HOMEPAGE_HTML;
                    homepageRes.setBody(bodyContent, "text/html");
                    return homepageRes; });
    }
    server.listen(port, backlog, threadPoolSize);
    return 0;
}