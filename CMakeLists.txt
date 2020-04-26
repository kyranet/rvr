cmake_minimum_required(VERSION 3.16)
project(rvr)

set(CMAKE_CXX_STANDARD 14)

add_executable(rvr
        02-module/gai.cpp
        shared/addrinfo.cpp
        shared/tcp.cpp
        shared/udp.cpp 02-module/time_server.cpp)
