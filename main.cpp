#include <iostream>
#include "chatServer.hpp"

int main(){
    
    try{
        chatServer server{};

        server.run();
    }
    catch(std::exception& exception){
        std::cerr << "Exception thrown: " << exception.what() << std::endl;
        // ensures stack is unwound and destructors are called
        
        return -1;
    }
    return 0;
}
