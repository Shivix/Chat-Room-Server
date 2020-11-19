#include <iostream>
#include "chatServer.hpp"

int main(){
    
    chatServer server;

    try{
        server.run();
    }
    catch(std::exception& exception){
        std::cerr << exception.what() << std::endl;
    }
}
