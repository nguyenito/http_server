# http_server
##### 1. Create the server object
```
HttpServer server;
```

##### 2. Adding Get Method for your request uri and return the HttpResponse object
```
std::string homePageHtmlBody = "<HTML><HEAD><TITLE>Albert Einstein Quotes</TITLE></HEAD><H1>Logic will get you from A to B. Imagination will take you everywhere</H1></HTML>";
server.GET("/", [&homePageHtmlBody](){
                   HttpResponse homepageRes(HttpStatusCode::OK);
                   homepageRes.setBody(homePageBody, "text/html");
                   return homepageRes; });
```

##### 3. Start listening on port, backlog(number of connection queue), number of thread pool size for concurrent reading socket
```
int port = 8080;
int backlog = 1000;
size_t threadPoolSize = 5;
server.listen(port, backlog, threadPoolSize);
```

#### Feature supported
Not support Keep-Alive connection yet
Currently only support GET method and header with Content-Lenght when client sending Http message with body

