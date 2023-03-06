cmake_minimum_required(VERSION 3.0.0)
project(http_server VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HTTP_SERVER_SOURCE src/http_header src/http_message src/http_request.cpp src/http_response.cpp src/server_utils.cpp src/server_socker.cpp src/http_server.cpp)
add_executable(http_server ${HTTP_SERVER_SOURCE} src/main.cpp)
add_executable(test_client  ${HTTP_SERVER_SOURCE} test/client.cpp)
