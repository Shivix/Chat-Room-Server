#ifndef CHATCLIENT_CHATCLIENT_HPP
#define CHATCLIENT_CHATCLIENT_HPP

#include <netinet/in.h>
#include <string>
#include <poll.h>
#include "../MessageProtocol/messageProtocol.hpp"
#include <array>
#include <unistd.h>

class chatClient{
    
    static constexpr uint16_t serverPort{12345};
    static constexpr size_t maxMessageSize{2048};
    const std::string_view serverIP{"127.0.0.1"}; // currently localhost 
    sockaddr_in serverAddress{}; // hint structure for full server address
    std::string username{};

public:
    std::string activeChatRoom{"Main"};
    std::array<pollfd, 2> fdSet{pollfd{STDIN_FILENO, POLLIN, 0}, {socket(PF_INET, SOCK_STREAM, 0), POLLIN, 0}};
    const int serverFD{fdSet[1].fd};
public:
    chatClient();
    ~chatClient();
    chatClient(const chatClient&) = delete;
    chatClient& operator=(chatClient&) = delete;
    
    void connectToServer() const;
    void sendMessage(messageProtocol::messageType, const std::string&, const std::string&) const;
    void receiveMessage() const;
private:
    void setUsername();
    [[nodiscard]] bool validateUsername() const;
    static void printMessage(const messageProtocol&);
};


#endif //CHATCLIENT_CHATCLIENT_HPP
