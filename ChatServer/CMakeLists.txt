cmake_minimum_required(VERSION 3.16)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)

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

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT MINGW) AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0))
    target_link_libraries(ChatServer -lstdc++fs)
endif()