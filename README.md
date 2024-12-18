**Multi-User Chat System**

A simple multi-user chat system implemented using C++ and sockets. The system allows multiple clients to connect to a server, send messages to each other, and broadcast messages to all clients in real-time.

Features:-
Server listens for incoming client connections.
Messages from one client are broadcast to all other connected clients.
Handles client disconnections and ensures that other clients are notified.
Basic client-server communication over TCP using sockets.
Files:-
server.cpp: The server code that manages client connections and broadcasts messages.
client.cpp: The client code that connects to the server, sends messages, and receives messages from the server.
chat_history.txt: It stores the conversation between the clients.
