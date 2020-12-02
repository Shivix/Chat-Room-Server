#include "messageProtocol.hpp"
#include <utility>

messageProtocol::messageProtocol(messageType type, std::string recipient, std::string sender, std::string message):
type{type},
recipient{std::move(recipient)},
sender{std::move(sender)},
message{std::move(message)},
length{this->message.size() + this->recipient.size() + this->sender.size()} {
    mergeData();
}

messageProtocol::messageProtocol(std::string data): 
mergedData{std::move(data)}{
    splitData();
}

void messageProtocol::mergeData(){
    std::string result{};
    result.append(std::to_string(length)); // append length first so the recipients client knows how much more to expect
    result.push_back(splitter);
    if (type == messageType::chatRm){
        result.append("chatRm");
    }
    else if (type == messageType::direct){
        result.append("direct");
    }
    else if (type == messageType::notify){
        result.append("notify");
    }
    result.push_back(splitter);
    result.append(recipient);
    result.push_back(splitter);
    result.append(sender);
    result.push_back(splitter);
    result.append(message);
    mergedData = result;
}

void messageProtocol::splitData(){
    size_t currentSplitterPos{mergedData.find(':')};
    auto setMessagePart = [&currentSplitterPos, this](auto& messagePart){
        size_t previousSplitterPos = currentSplitterPos;
        currentSplitterPos = mergedData.find(':', currentSplitterPos + 1);
        messagePart = mergedData.substr(previousSplitterPos + 1, (currentSplitterPos - previousSplitterPos) - 1);
    };
    
    length = std::atoi(mergedData.substr(0, currentSplitterPos).data());
    
    std::string typeString{};
    setMessagePart(typeString);
    if (typeString == "chatRm"){
        type = messageType::chatRm;
    }
    else if (typeString == "direct"){
        type = messageType::direct;
    }
    else{
        type = messageType::notify;
    }
    
    setMessagePart(recipient);
    setMessagePart(sender);
    setMessagePart(message);
}

std::string messageProtocol::getMessageWithSender() const{
    std::string result{sender};
    result.append(": ");
    result.append(message);
    return result;
}

bool messageProtocol::verifyPayload(std::string payload){
    payload.erase(std::remove(payload.begin(), payload.end(), '\0'), payload.end());
    size_t splitterPos{payload.find(':')};
    size_t expectedLength = std::atoi(payload.substr(0, splitterPos).data());
    if (payload.size() - splitterPos == expectedLength + typeLength + 4){ // checks the size after the length considering the 4 splitters
        return true;
    }
    else{
        return false;
    }
}
