#ifndef CHATSERVER_MESSAGEPROTOCOL_HPP
#define CHATSERVER_MESSAGEPROTOCOL_HPP

#include <cstddef>
#include <string>

class messageProtocol{
public:
    enum class messageType{
        chatRm,
        direct,
        notify
    };
    static constexpr size_t typeLength{6};

public:
    messageType type;
    std::string recipient;
    std::string sender;
    std::string message;
    size_t length;
    
public:
    std::string mergedData;
    messageProtocol(messageType, std::string, std::string, std::string); // constructor for creating message based on seperate data AND one for creating seperate data based on string
    explicit messageProtocol(std::string);
    std::string getMessageWithSender() const;
    static bool verifyPayload(std::string);
    
private:
    void mergeData();
    void splitData();
    static constexpr char splitter{':'};
};


#endif //CHATSERVER_MESSAGEPROTOCOL_HPP
