cmake_minimum_required(VERSION 3.16)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS -pthread)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(ChatServer main.cpp chatServer.cpp chatServer.hpp chatClient.cpp chatClient.hpp)