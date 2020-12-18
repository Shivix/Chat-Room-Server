#include <stdexcept>
#include <iostream>
#include <arpa/inet.h>
#include <chrono>
#include "../include/chatServer.hpp"

chatServer::chatServer(){
    if (listenFD.fd == -1){
        throw std::runtime_error("Failed to create socket");
    }
    std::cout << "Server socket initialized" << std::endl;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(PF_INET, serverIP.data(), &serverAddress.sin_addr);// sets the IP address that is used to connect to the server

    if (bind(listenFD.fd, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1){ // binds the address stored in serverAddress to the socket
        throw std::runtime_error("Failed to bind IP");
    }

    if (listen(listenFD.fd, SOMAXCONN) == -1){ // sets the socket to listen for incoming clients
        throw std::runtime_error("Server failed to listen");
    }
    
    fdSet.emplace_back(cinFD);// create file descriptor for std::cin
    fdSet.emplace_back(listenFD);

    logFile.open(logFilePath);
    if (!logFile.is_open()){
        throw std::runtime_error("Could not open log file");
    }
}

chatServer::~chatServer(){
    // close socket and log file
    close(listenFD.fd);
    logFile.close();
}

void chatServer::run(){
    while(true){
        int amountSet{poll(fdSet.data(), fdSet.size(), -1)};
        
        if(amountSet == -1){
            throw std::runtime_error("Error occurred while polling file descriptors");
        }
        for(size_t i{0}; i < fdSet.size(); ++i){ // range based for loop cannot be used here since we add to the vector in addClient()
            if (fdSet[i].fd == cinFD.fd && fdSet[i].revents != 0){
                std::string input{};
                std::getline(std::cin, input);
                if (input == "exit()"){
                    break;
                }
            }
            else if (fdSet[i].fd == listenFD.fd && fdSet[i].revents != 0){// checks if it was one of the FDs that had an event
                addClient();
            }
            else{
                if (fdSet[i].revents != 0){
                    relayMessage(*std::find_if(clientList.begin(), clientList.end(), [&, i](const chatClient& input){
                        return input.clientFD == fdSet[i].fd;
                    }));
                }
            }
        }
    }
}

void chatServer::addClient(){
    // create client within the list and add their FD to the set
    clientList.emplace_back(listenFD.fd);
    fdSet.emplace_back(pollfd{clientList.back().clientFD, POLLIN, 0});
    // set file descriptor for the client socket and add it to vector
    // personal welcome message
    std::string welcomeMessage{"You have successfully connected "};
    welcomeMessage.append(clientList.back().username); // personalizes the welcome message
    messageProtocol welcomeNotification{messageProtocol::messageType::notify, clientList.back().username, "Server", welcomeMessage};
    send(clientList.back().clientFD, welcomeNotification.mergedData.data(), welcomeNotification.mergedData.size() + 1, 0);
    
    // notification message for other users in the chatroom
    std::string notificationMessage{clientList.back().username};
    notificationMessage.append(" has entered the chat");
    messageProtocol payload{messageProtocol::messageType::notify, "Main", "Server", notificationMessage};
    for (auto client = clientList.begin(); client != clientList.end() - 1; ++client){
        send(client->clientFD, payload.mergedData.data(), payload.mergedData.size() + 1, 0);
    }
}

void chatServer::removeClient(const chatClient& client){
    std::cout << client.username << " has disconnected" << std::endl;
    // create notification that someone left and send to all users
    std::string notificationMessage{client.username};
    notificationMessage.append(" has left the chat");
    messageProtocol payload{messageProtocol::messageType::notify, "Main", "Server", notificationMessage};
    
    fdSet.erase(std::find_if(fdSet.begin(), fdSet.end(), [&client](auto input){
        return input.fd == client.clientFD;
    }));
    clientList.erase(std::find(clientList.begin(), clientList.end(), client)); // remove the user from the chatroom so we can just send to everyone
    for (auto&& i: clientList){
        send(i.clientFD, payload.mergedData.data(), payload.mergedData.size() + 1, 0);
    }
}

void chatServer::relayMessage(const chatClient& sender){ // only happens when message is waiting to be received
    
    const auto message{receivePayload(sender)};
    if (!message.has_value()){
        return;
    }
    
    if (message->recipient == "Main"){
        // output chatroom messages serverside
        auto currentTime {std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        std::cout << std::put_time(std::gmtime(&currentTime), "%H:%M ") << message->getMessageWithSender() << std::endl; // displays message server side if it wasn't a private message
        
        // add messages log file
        logFile << std::ctime(&currentTime) << message->getMessageWithSender() << std::endl;
        
        // loop through clientList to send message to each client
        for(auto&& client: clientList){
            if (client != sender){
                send(client.clientFD, message->mergedData.data(), message->mergedData.size(), 0);
            }
        }
    }
    else{
        send(std::find_if(clientList.begin(), clientList.end(), [message](const chatClient& client){
            return client.username == message->recipient;
        })->clientFD, message->mergedData.data(), message->mergedData.size(), 0);
    }
}

std::optional<messageProtocol> chatServer::receivePayload(const chatClient& sender){
    std::string mainBuffer{};
    do{
        auto bytesReceived{recv(sender.clientFD, mainBuffer.data(), maxMessageSize, MSG_PEEK)}; // peeks at the message and stores its size
        if (bytesReceived == -1){
            throw std::runtime_error("Failed to receive payload");
        }
        std::string tempBuffer(bytesReceived, '\0'); // buffer must be resized before recv because the function interfaces with the raw array. reserve does not work
        recv(sender.clientFD, tempBuffer.data(), bytesReceived, 0); // waits for message to be sent
        
        if (bytesReceived == 0){
            removeClient(sender);
            return {};
        }
        else{
            mainBuffer += tempBuffer;
        }
    }
    while (!messageProtocol::verifyPayload(mainBuffer));
    return messageProtocol{mainBuffer};
}

