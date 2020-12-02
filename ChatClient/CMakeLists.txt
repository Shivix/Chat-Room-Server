cmake_minimum_required(VERSION 3.16)
project(ChatClient)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(ChatClient main.cpp chatClient.cpp chatClient.hpp ../MessageProtocol/messageProtocol.cpp ../MessageProtocol/messageProtocol.hpp)