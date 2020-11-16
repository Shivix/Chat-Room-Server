#ifndef CHATSERVER_CHATCLIENT_HPP
#define CHATSERVER_CHATCLIENT_HPP

#include <array>
#include <netinet/in.h>
#include <netdb.h>

class chatClient{

    std::array<char, NI_MAXHOST> host{};
    std::array<char, NI_MAXSERV> service{}; 
    
public:
    explicit chatClient(int);
    ~chatClient();
    chatClient(const chatClient&) = delete;
    chatClient& operator=(chatClient&) = delete;
    chatClient(chatClient&&) noexcept = default;
    chatClient& operator=(chatClient&&) = default;
    
    int clientSocket{};
    sockaddr_in clientAddress{};
    socklen_t clientSize{sizeof(clientAddress)};

};


#endif //CHATSERVER_CHATCLIENT_HPP
