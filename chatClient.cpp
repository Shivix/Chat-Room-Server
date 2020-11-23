#include <iostream>
#include <arpa/inet.h>
#include <zconf.h>
#include "chatClient.hpp"


chatClient::chatClient(int serverSocket){
    
    clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientSize); // listens for and accepts incoming client request
    
    if (clientSocket == -1){
        throw std::runtime_error("Failed to connect client");
    }

    auto usernameSize = recv(clientSocket, username.data(), 50, MSG_PEEK);
    
    username.resize(usernameSize);
    if (recv(clientSocket, username.data(), usernameSize, 0) < 1){ // receive username, maximum 50 characters
        throw std::runtime_error("Could not receive username");
    }
    
    int result{getnameinfo(reinterpret_cast<const sockaddr*>(&clientAddress), sizeof(clientAddress), host.data(), NI_MAXHOST, service.data(), NI_MAXSERV, 0)};

    if (result){// displays info serverside about connected client
        std::cout << username << " has connected on " << service.front() << std::endl;
    }
    else{
        inet_ntop(PF_INET, &clientAddress.sin_addr, host.data(), NI_MAXHOST);
        std::cout << username << " has connected on port: " << clientAddress.sin_port << std::endl;
    }
    
    clientFD.fd = clientSocket;
    clientFD.events = POLLIN;
}

chatClient::~chatClient(){
    close(clientSocket);
}

chatClient::chatClient(chatClient&& other) noexcept: // move constructor must change the socket of moved object to prevent early closing
    host(other.host),
    service(other.service),
    clientSocket(std::exchange(other.clientSocket, -1)),
    clientAddress(other.clientAddress),
    clientSize(other.clientSize),
    username(std::move(other.username)),
    clientFD(other.clientFD) {}

chatClient& chatClient::operator=(chatClient&& other) noexcept {
    if(this != &other){
        host = other.host;
        service = other.service;
        clientSocket = std::exchange(other.clientSocket, -1);
        clientAddress = other.clientAddress;
        clientSize = other.clientSize;
        username = std::move(other.username);
        clientFD = other.clientFD;
    }
    return *this;
}
