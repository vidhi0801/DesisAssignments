#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;  // This allows us to use the standard library without needing to prefix with std::

// Define the port number the server will listen on
#define PORT 8080

// Global variables to manage clients and chat history
vector<int> clients;  // List of connected clients (file descriptors)
vector<string> chat_history;  // Vector to hold chat messages
mutex clients_mutex;  // Mutex to protect access to the clients list
mutex chat_mutex;  // Mutex to protect access to chat history

// Function to broadcast a message to all clients except the sender
void broadcast_message(const string& message, int sender_fd) {
    lock_guard<mutex> lock(clients_mutex);  // Ensure thread-safe access to clients list
    // Loop through all clients and send the message (excluding the sender)
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            size_t total_sent = 0;
            size_t to_send = message.size();
            const char* msg = message.c_str();
            // Keep sending until the full message is sent
            while (total_sent < to_send) {
                int sent = send(client_fd, msg + total_sent, to_send - total_sent, 0);
                if (sent <= 0) {
                    break;  // Exit if sending fails
                }
                total_sent += sent;  // Update the total sent message size
            }
        }
    }
}

// Function to handle communication with a single client
void handle_client(int client_fd) {
    char buffer[1024];  // Buffer to hold received data
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        // Receive a message from the client
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            // If no data is received (or error), the client has disconnected
            close(client_fd);  // Close the connection
            // Remove the client from the clients list
            lock_guard<mutex> lock(clients_mutex);
            clients.erase(remove(clients.begin(), clients.end(), client_fd), clients.end());
            cout << "Client " << client_fd << " disconnected.\n";
            break;  // Exit the loop
        }
        // Create the message string to be broadcasted
        string message = "Client " + to_string(client_fd) + ": " + buffer + "\n";
        cout << message;  // Print the message to the console
        broadcast_message(message, client_fd);  // Broadcast the message to other clients

        // Save the chat message to the chat history
        {
            lock_guard<mutex> lock(chat_mutex);  // Ensure thread-safe access to chat history
            chat_history.push_back(message);  // Add the message to chat history
        }
    }
}

// Function to periodically save chat history to a file
void save_chat_history() {
    while (true) {
        this_thread::sleep_for(chrono::seconds(30));  // Save every 30 seconds
        ofstream out("chat_history.txt", ios::app);  // Open the chat history file for appending
        if (out.is_open()) {
            lock_guard<mutex> lock(chat_mutex);  // Ensure thread-safe access to chat history
            // Write all messages in chat_history to the file
            for (const string& message : chat_history) {
                out << message;  // Write each message to the file
            }
            cout << "Chat history saved.\n";
            chat_history.clear();  // Clear chat history after saving
        } else {
            cerr << "Failed to open chat_history.txt for saving.\n";
        }
    }
}

int main() {
    // Create the server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        return -1;  // Exit if socket creation fails
    }

    // Set up the server address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any address
    address.sin_port = htons(PORT);  // Use the defined port

    // Bind the socket to the server address
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;  // Exit if bind fails
    }

    // Start listening for incoming connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;  // Exit if listen fails
    }

    cout << "Server listening on port " << PORT << endl;

    // Start the chat history saving thread
    thread(save_chat_history).detach();  // Run save_chat_history function in a detached thread

    // Main loop to accept client connections
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);  // Accept a new client connection
        if (client_fd < 0) {
            perror("Accept failed");
            continue;  // Skip to the next iteration if accept fails
        }
        cout << "New client connected: " << client_fd << endl;

        {
            lock_guard<mutex> lock(clients_mutex);  // Ensure thread-safe access to clients list
            clients.push_back(client_fd);  // Add the new client to the clients list
        }

        // Start a new thread to handle communication with the client
        thread(handle_client, client_fd).detach();
    }

    // Close the server socket when done
    close(server_fd);
    return 0;
}


