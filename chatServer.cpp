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

    if (bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1){ // binds the address stored in serverAddress to the socket
        throw std::runtime_error("Failed to bind IP");
    }

    if (listen(serverSocket, SOMAXCONN) == -1){ // sets the socket to listen for incoming clients
        throw std::runtime_error("Server failed to listen");
    }
    listenFD = pollfd{serverSocket, POLLIN, 0}; // file descriptor to store events associated with the listening socket
}

chatServer::~chatServer(){
    // clean up sockets
    close(serverSocket);
}

void chatServer::run(){
    while(true){
        
        if (poll(&listenFD, 1, 0)){// checks if it was one of the FDs that had an event
            addClient();                                                         
        }                                                                        
        int amountSet{0};
        for(auto&& client: clientList){
            amountSet += poll(&client.clientFD, 1, 0);
        }
        
        if(amountSet == -1){
            throw std::runtime_error("Error occurred while polling file descriptors");
        }
        else if(amountSet == 0){ // no messages have been sent and no clients waiting to be accepted
            continue;
        }
        else{
            for(auto&& client: clientList){
                if (client.clientFD.revents != 0){
                    relayMessage(client);
                }
            }
        }
    }
}

void chatServer::addClient(){
    
    clientList.emplace_back(serverSocket); // TODO: old client disconnects here
    // set file descriptor for the client socket and add it to vector
    
    std::string_view welcomeMessage{"Welcome to the chat room\n"};
    send(clientList.back().clientSocket, welcomeMessage.data(), welcomeMessage.size() + 1, 0);
}

void chatServer::removeClient(const chatClient& client){
    clientList.erase(std::find(clientList.begin(), clientList.end(), client));
}

void chatServer::relayMessage(const chatClient& sender){ // only happens when message is waiting to be received
    std::string messageBuffer{};
    messageBuffer.resize(maxMessageSize);// TODO

    int bytesRecieved = recv(sender.clientSocket, messageBuffer.data(), maxMessageSize, 0); // waits for message to be sent
    
    if (bytesRecieved == 0){ // signifies a disconnect by the client
        removeClient(sender);
        std::cout << sender.username << " has disconnected" << std::endl;
    }
    else if (bytesRecieved > 0){ // checks if the message was successfully received
        std::cout << sender.username << ": " << messageBuffer << std::endl; // displays message server side

        std::string message{sender.username}; // adds the username to the beginning of the message
        message.pop_back();
        message.append(": ");
        message.append(messageBuffer);
        
        // loop through clientList to send message to each client
        for(auto&& client: clientList){
            if (client != sender){
                send(client.clientSocket, message.data(), message.size() + 1, 0);
            }
        }
    }
    else{
        throw std::runtime_error("Failed to receive message");
    }
}

