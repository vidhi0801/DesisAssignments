#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8080 // Define the port number to connect to the server

// Function to receive messages from the server
void receive_messages(int sock) {
    char buffer[1024];  // Buffer to store incoming messages
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer before receiving new data
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);  // Receive a message from the server
        if (bytes_received <= 0) {  // If the connection is lost or no data is received
            std::cout << "Disconnected from server.\n";  // Notify the user
            close(sock);  // Close the socket connection
            break;  // Exit the loop as the connection is closed
        }
        std::cout << buffer << std::endl;  // Print the received message to the console
    }
}

int main() {
    // Create a socket for the client
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {  // If socket creation fails
        perror("Socket creation failed");  // Print error message
        return -1;
    }

    // Set up the server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;  // Set address family to IPv4
    server_address.sin_port = htons(PORT);  // Set port number to connect to
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);  // Convert server IP address to binary format

    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");  // If connection fails, print error message
        close(sock);  // Close the socket
        return -1;
    }

    std::cout << "Connected to the server.\n";  // Notify the user of a successful connection

    // Start a new thread to receive messages from the server
    std::thread(receive_messages, sock).detach();  // Detach the thread to run independently

    std::string message;
    while (true) {
        std::getline(std::cin, message);  // Get user input
        if (message == "exit") {  // If the user types "exit"
            close(sock);  // Close the socket connection
            break;  // Exit the loop and terminate the program
        }
        send(sock, message.c_str(), message.size(), 0);  // Send the message to the server
    }

    return 0;
}

