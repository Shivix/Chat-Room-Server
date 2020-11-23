#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "chatServer.hpp"

chatServer::chatServer():
        listenFD(pollfd{serverSocket, POLLIN, 0})
{
    if (serverSocket == -1){
        throw std::runtime_error("Failed to create socket");
    }
    std::cout << "Server socket initialized" << std::endl;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(PF_INET, serverIP.data(), &serverAddress.sin_addr);// sets the IP address that is used to connect to the server

    if (bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1){ // binds the address stored in serverAddress to the socket
        throw std::runtime_error("Failed to bind IP");
    }

    if (listen(serverSocket, SOMAXCONN) == -1){ // sets the socket to listen for incoming clients
        throw std::runtime_error("Server failed to listen");
    }
    
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
    
    clientList.emplace_back(serverSocket);
    // set file descriptor for the client socket and add it to vector
    
    std::string welcomeMessage{"Welcome to the chat room "};
    welcomeMessage.append(clientList.back().username);
    send(clientList.back().clientSocket, welcomeMessage.data(), welcomeMessage.size() + 1, 0);
    
    std::string notification{clientList.back().username};
    notification.append(" has entered the chat");
    for (auto client = clientList.begin(); client != clientList.end() - 1; ++client){
        send(client->clientSocket, notification.data(), welcomeMessage.size() + 1, 0);
    }
}

void chatServer::removeClient(const chatClient& client){
    clientList.erase(std::find(clientList.begin(), clientList.end(), client));

    std::cout << client.username << " has disconnected" << std::endl;
    
    std::string notification{client.username};
    notification.append(" has left the chat");
    
    for (auto&& i: clientList){
        send(i.clientSocket, notification.data(), notification.size() + 1, 0);
    }

}

void chatServer::relayMessage(const chatClient& sender){ // only happens when message is waiting to be received
    std::string messageBuffer{};
    messageBuffer.resize(maxMessageSize); // buffer must be initialized with default values with a size bigger than the message because C

    auto bytesReceived{recv(sender.clientSocket, messageBuffer.data(), maxMessageSize, 0)}; // waits for message to be sent
    
    if (bytesReceived == 0){ // signifies a disconnect by the client
        removeClient(sender);
    }
    else if (bytesReceived > 0){ // checks if the message was successfully received
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

