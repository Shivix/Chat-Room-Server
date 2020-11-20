#ifndef CHATSERVER_CHATSERVER_HPP
#define CHATSERVER_CHATSERVER_HPP


#include <sys/socket.h>
#include <netinet/in.h>
#include <array>
#include <netdb.h>
#include <vector>
#include <poll.h>
#include "chatClient.hpp"


class chatServer{ 
    
    static constexpr uint16_t port{12345};
    static constexpr size_t maxMessageSize{2048};
    int serverSocket{socket(PF_INET, SOCK_STREAM, 0)}; // endpoint for all incoming and outgoing data

    sockaddr_in serverAddress{}; // struct with address info to bind the socket
    
    std::vector<chatClient> clientList;

    pollfd listenFD;
    
public:
    chatServer();
    ~chatServer();
    chatServer(const chatServer&) = delete;
    chatServer& operator=(chatServer) = delete;
    
    void run();
    void addClient();
    void removeClient(const chatClient&);
    void relayMessage(const chatClient&);
};


#endif //CHATSERVER_CHATSERVER_HPP
