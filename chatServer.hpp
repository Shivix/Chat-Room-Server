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
    static constexpr size_t maxMessageSize{4096};
    int serverSocket{socket(PF_INET, SOCK_STREAM, 0)}; // creates socket for communication with server using TCP to ensure data isn't lost

    sockaddr_in serverAddress{}; // struct with address info to bind the socket
    
    std::vector<chatClient> clientList;

    std::vector<pollfd> fileDescriptors; // TODO: add to client.hpp and have just single pollfd for listener here?
public:
    chatServer();
    ~chatServer();
    chatServer(const chatServer&) = delete;
    chatServer& operator=(chatServer) = delete;
    
    void run();
    void addClient();
    void removeClient(int);
    void relayMessage(int);
};


#endif //CHATSERVER_CHATSERVER_HPP
