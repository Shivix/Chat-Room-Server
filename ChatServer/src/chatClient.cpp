#include <iostream>
#include <arpa/inet.h>
#include <zconf.h>
#include "../include/chatClient.hpp"
#include "../../MessageProtocol/messageProtocol.hpp"

chatClient::chatClient(int serverSocket){
    
    clientFD = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientSize); // listens for and accepts incoming client request
    
    if (clientFD == -1){
        throw std::runtime_error("Failed to connect client");
    }
    
    setUsername();
    
    int result{getnameinfo(reinterpret_cast<const sockaddr*>(&clientAddress), sizeof(clientAddress), host.data(), NI_MAXHOST, service.data(), NI_MAXSERV, 0)};

    if (result){// displays info serverside about connected client
        std::cout << username << " has connected on " << service.front() << std::endl;
    }
    else{
        inet_ntop(PF_INET, &clientAddress.sin_addr, host.data(), NI_MAXHOST);
        std::cout << username << " has connected on port: " << clientAddress.sin_port << std::endl;
    }
    
}

chatClient::~chatClient(){
    close(clientFD);
}

chatClient::chatClient(chatClient&& other) noexcept: // move constructor must change the socket of moved object to prevent early closing
    host{other.host},
    service{other.service},
    clientFD{std::exchange(other.clientFD, -1)},
    username{std::move(other.username)},
    clientAddress{other.clientAddress},
    clientSize{other.clientSize} {}

chatClient& chatClient::operator=(chatClient&& other) noexcept {
    if(this != &other){
        host = other.host;
        service = other.service;
        clientFD = std::exchange(other.clientFD, -1);
        username = std::move(other.username);
        clientAddress = other.clientAddress;
        clientSize = other.clientSize;
    }
    return *this;
}

void chatClient::setUsername(){
    std::string usernameBuffer{};
    do{
        std::string buffer{};
        auto usernameSize{recv(clientFD, buffer.data(), 20, MSG_PEEK)};
        buffer.resize(usernameSize);
        if (recv(clientFD, buffer.data(), usernameSize, 0) < 1){ // receive username, maximum 50 characters
            throw std::runtime_error("Could not receive username");
        }
        usernameBuffer += buffer;
    }
    while (!messageProtocol::verifyPayload(usernameBuffer));
    const messageProtocol payload{usernameBuffer};
    username = payload.message;
}
