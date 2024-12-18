#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

#define PORT 8080 // Define the port number on which the server will listen

std::vector<int> clients;  // Vector to hold client file descriptors
std::mutex clients_mutex;  // Mutex to synchronize access to the client list

// Function to broadcast a message to all clients except the sender
void broadcast_message(const std::string& message, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);  // Lock to ensure thread safety when modifying the client list
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {  // Don't send the message to the sender
            size_t total_sent = 0;
            size_t to_send = message.size();
            const char* msg = message.c_str();
            while (total_sent < to_send) {
                int sent = send(client_fd, msg + total_sent, to_send - total_sent, 0);
                if (sent <= 0) {
                    break;  // If sending fails, break out of the loop
                }
                total_sent += sent;  // Update how much of the message has been sent
            }
        }
    }
}

// Function to handle communication with a client
void handle_client(int client_fd) {
    char buffer[1024];  // Buffer to store incoming messages
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);  // Receive a message from the client
        if (bytes_received <= 0) {  // If no data is received or there is an error
            close(client_fd);  // Close the client connection
            std::lock_guard<std::mutex> lock(clients_mutex);  // Lock to safely modify the client list
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());  // Remove client from list
            std::cout << "Client " << client_fd << " disconnected.\n";  // Log disconnection
            break;
        }
        std::string message = "Client " + std::to_string(client_fd) + ": " + buffer + "\n";  // Format the message with the client id
        std::cout << message;  // Print the message on the server
        broadcast_message(message, client_fd);  // Broadcast the message to all other clients
    }
}

int main() {
    // Create a socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");  // If socket creation fails, print error and exit
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;  // Set address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Bind to any available network interface
    address.sin_port = htons(PORT);  // Set the port to listen on

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");  // If bind fails, print error and exit
        close(server_fd);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");  // If listen fails, print error and exit
        close(server_fd);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        // Accept an incoming client connection
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("Accept failed");  // If accept fails, print error and continue to accept other clients
            continue;
        }
        std::cout << "New client connected: " << client_fd << std::endl;

        // Add the client to the client list in a thread-safe manner
        {
            std::lock_guard<std::mutex> lock(clients_mutex);  // Lock to safely modify the shared list
            clients.push_back(client_fd);  // Add the client to the list
        }

        // Create a new thread to handle communication with this client
        std::thread(handle_client, client_fd).detach();  // Detach the thread so it runs independently
    }

    close(server_fd);  // Close the server socket when the server shuts down
    return 0;
}

