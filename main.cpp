#include <iostream>
#include "chatServer.hpp"

int main(){
    
    chatServer server;
    server.addClient();
//    server.addClient();
    server.run();
}
