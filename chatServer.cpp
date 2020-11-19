#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "chatServer.hpp"

chatServer::chatServer(){
    if (serverSocket == -1){
        throw std::runtime_error("Failed to create socket");
    }
    std::cout << "Server socket initialized" << std::endl;

    serverAddress.sin_family = PF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1){
        throw std::runtime_error("Failed to bind IP");
    }

    if (listen(serverSocket, SOMAXCONN) == -1){
        throw std::runtime_error("Server failed to listen");
    }
    fileDescriptors.emplace_back(pollfd{serverSocket, POLLIN, 0});
}

chatServer::~chatServer(){
    // clean up sockets
    close(serverSocket);
    
    for(auto&& client: clientList){
        close(client.clientSocket);
    }
    
}

void chatServer::run(){
    while(true){
        
        auto amountSet = poll(fileDescriptors.data(), fileDescriptors.size(), 0);
        
        if(amountSet == -1){
            throw std::runtime_error("Error occurred while polling file descriptors");
        }
        else if(amountSet == 0){ // no messages have been sent and no clients waiting to be accepted
            continue;
        }
        else{
        // check which FDs are set TODO:   
        for(auto&& i: fileDescriptors){
            if(i.fd == serverSocket){
                if (i.revents != 0){// checks if it was one of the FDs that had an event
                    addClient();
                }
            }
            else{
                if (i.revents != 0){
                    relayMessage(i.fd);
                }
            }
        }
        // add client if listener FD is set
        
        // relay message if client FD is set
        }
    }
}

void chatServer::addClient(){
    
    clientList.emplace_back(serverSocket); // TODO: old client disconnects here
    // set file descriptor for the client socket and add it to vector
    
    fileDescriptors.emplace_back(pollfd{clientList.back().clientSocket, POLLIN, 0}); 
    
    std::string_view welcomeMessage{"Welcome to the chat room\n"};
    send(clientList.back().clientSocket, welcomeMessage.data(), welcomeMessage.size() + 1, 0);
}

void chatServer::removeClient(int /*clientSocket*/){
    // erase from clientList
}

void chatServer::relayMessage(int clientSocket){ // only happens when message is waiting to be received
    std::string message{};
    message.resize(maxMessageSize);// TODO

    int bytesRecieved = recv(clientSocket, message.data(), maxMessageSize, 0); // waits for message to be sent

    if (bytesRecieved > 0){ // TODO: check bytes received outside of function?
        std::cout << "Message: " << message.data() << std::endl; // displays message server side
        // loop through clientList to send message to each client
        for(auto&& client: clientList){ // TODO: dont relay message back to sender
            //sendto(clientSocket, message.data(), message.size(), 0, reinterpret_cast<const sockaddr*>(&client.clientAddress), client.clientSize);
            send(client.clientSocket, message.data(), bytesRecieved, 0);
        }
    }
}

