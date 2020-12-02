cmake_minimum_required(VERSION 3.16)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS -pthread)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(SRC
        src/main.cpp
        src/chatClient.cpp
        src/chatServer.cpp
        ../MessageProtocol/messageProtocol.cpp)

set(INCLUDE
        include/chatClient.hpp
        include/chatServer.hpp
        ../MessageProtocol/messageProtocol.hpp)

add_executable(ChatServer ${SRC} ${INCLUDE})