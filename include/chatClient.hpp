#ifndef CHATSERVER_CHATCLIENT_HPP
#define CHATSERVER_CHATCLIENT_HPP

#include <array>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>

class chatClient{

    std::array<char, NI_MAXHOST> host{};
    std::array<char, NI_MAXSERV> service{};
    
public:
    explicit chatClient(int);
    ~chatClient();
    chatClient(const chatClient&) = delete; // any copies would be an error so avoided by deleting the copy ctor
    chatClient& operator=(chatClient&) = delete; 
    chatClient(chatClient&&) noexcept;
    chatClient& operator=(chatClient&&) noexcept;
    
    constexpr bool operator!=(const chatClient& other) const{
        return !(*this == other);
    }

    constexpr bool operator==(const chatClient& other) const{
        return clientFD.fd == other.clientFD.fd;
    }
    
    void setUsername();
    pollfd clientFD{};
    std::string username{};
private:
    sockaddr_in clientAddress{};
    socklen_t clientSize{sizeof(clientAddress)};
};


#endif //CHATSERVER_CHATCLIENT_HPP
