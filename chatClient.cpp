#include <iostream>
#include <arpa/inet.h>
#include <zconf.h>
#include "chatClient.hpp"


chatClient::chatClient(int serverSocket){
    
    clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientSize); // listens for and accepts incoming client request
    
    if (clientSocket == -1){
        throw std::runtime_error("Failed to connect client");
    }

    int result{getnameinfo(reinterpret_cast<const sockaddr*>(&clientAddress), sizeof(clientAddress), host.data(), NI_MAXHOST, service.data(), NI_MAXSERV, 0)};

    if (result){// displays info serverside about connected client
        std::cout << host.front() << " connected on " << service.front() << std::endl;
    }
    else{
        inet_ntop(PF_INET, &clientAddress.sin_addr, host.data(), NI_MAXHOST);
        std::cout << host.front() << " connected on port: " << clientAddress.sin_port << std::endl;
    }
}

chatClient::~chatClient(){
    close(clientSocket);
}

chatClient::chatClient(chatClient&& other) noexcept: // move constructor must change the socket of moved object to prevent early closing
    host(other.host),
    service(other.service),
    clientSocket(std::exchange(other.clientSocket, -1)),
    clientAddress(other.clientAddress),
    clientSize(other.clientSize) {}

chatClient& chatClient::operator=(chatClient&& other) noexcept {
    if(this != &other){
        host = other.host;
        service = other.service;
        clientSocket = std::exchange(other.clientSocket, -1);
        clientAddress = other.clientAddress;
        clientSize = other.clientSize;
    }
    return *this;
}
