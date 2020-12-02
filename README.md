# Chatroom Server and Client

This can be used to host a text based chatroom with private messaging

Uses poll() to manage multiple clients on one thread

# Compatibility
Currently only compatible with Linux

# Usage
Within the client you can type /"username" "Your Message" to send someone a private message

Type exit() at any time to quit the program, this works on both the server and the client

Times printed server side are all utc
Times printed client side are based on the clients os

If you wish to have a chatroom that people can connect to without being in the same LAN then
you will need to change the ip address which is currently set to localhost as well as the port must
be changed to an open port.