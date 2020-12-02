#include <stdexcept>
#include <iostream>
#include <zconf.h>
#include <arpa/inet.h>
#include "chatClient.hpp"

chatClient::chatClient(){
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    inet_pton(PF_INET, serverIP.data(), &serverAddress.sin_addr);

    setUsername();
    
    if(serverFD.fd < 0){
        throw std::runtime_error("Failed initialize client");
    }
}

void chatClient::connectToServer() const{
    if (connect(serverFD.fd, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1){
        throw std::runtime_error("Failed connect to server");
    }
    const messageProtocol payload{messageProtocol::messageType::notify, "Server", username, username};
    send(serverFD.fd, payload.mergedData.data(), payload.mergedData.size(), 0);
    receiveMessage(); // receive confirmation message from server will throw if not received
}

chatClient::~chatClient(){
    close(serverFD.fd);
}

void chatClient::sendMessage(messageProtocol::messageType type, const std::string& chatBuffer, const std::string& recipient) const{
    
    const messageProtocol message{type, recipient, username, chatBuffer};
    
    if (send(serverFD.fd, message.mergedData.data(), message.mergedData.size(), 0) == -1){
        throw std::runtime_error("Failed to send message");
    }
}

void chatClient::receiveMessage() const{
    std::string messageBuffer{};
    
    do{
        auto bytesReceived{recv(serverFD.fd, messageBuffer.data(), maxMessageSize, MSG_PEEK)};
        std::string tempBuffer(bytesReceived, '\0');
        recv(serverFD.fd, tempBuffer.data(), bytesReceived, 0);
        
        if (bytesReceived == 0){
            throw std::runtime_error("You have been disconnected from the server");
        }
        else if (bytesReceived == -1){
            throw std::runtime_error("Failed to receive message");
        }
        else{
            messageBuffer += tempBuffer;
        }
    }
    while (!messageProtocol::verifyPayload(messageBuffer));
    
    const messageProtocol payload{messageBuffer};
    printMessage(payload);
}

void chatClient::setUsername(){
    std::cout << "Please type your desired username: ";
    std::getline(std::cin, username);
    if (!validateUsername()){
        setUsername();
    }
}

bool chatClient::validateUsername() const{
    if (username.size() < 4 || username.size() > 20){
        std::cout << "Username must be between 4 and 20 characters" << std::endl;
        return false;
    }
    if(std::find_if(username.begin(), username.end(), [](auto character){return !std::isalnum(character);}) != username.end()){ // checks if username contains anything that is not a number or a letter
        std::cout << "Username cannot contain any special characters" << std::endl;
        return false;
    }
    return true;
}

void chatClient::printMessage(const messageProtocol& payload) {
    if (payload.sender == "Server"){
        std::cout << payload.message << std::endl;
    }
    else{
        std::cout << payload.getMessageWithSender() << std::endl;
    }
}

