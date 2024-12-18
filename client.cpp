#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <chrono>
#include <mutex>
#include <vector>

using namespace std;  // This allows us to use the standard library without needing to prefix with std::

// Define the port number to connect to the server
#define PORT 8080

mutex chat_history_mutex;  // Mutex for chat history file

// Function to receive messages from the server and save them to the file
void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);  // Receive data from the server
        if (bytes_received <= 0) {
            cout << "Disconnected from server.\n";
            close(sock);  // Close the socket when disconnected
            break;
        }
        cout << "Received: " << buffer << endl;

        // Save received messages to chat_history.txt
        {
            lock_guard<mutex> lock(chat_history_mutex);  // Ensure thread-safe access to the file
            ofstream out("chat_history.txt", ios::app);  // Open the file for appending
            if (out.is_open()) {
                out << "Received: " << buffer << endl;  // Write the received message to the file
            } else {
                cerr << "Failed to open chat_history.txt for saving.\n";  // Error handling if the file can't be opened
            }
        }
    }
}

// Function to save sent messages to the chat history file
void save_sent_message(const string& message) {
    {
        lock_guard<mutex> lock(chat_history_mutex);  // Ensure thread-safe access to the file
        ofstream out("chat_history.txt", ios::app);  // Open the file for appending
        if (out.is_open()) {
            out << "Sent: " << message << endl;  // Write the sent message to the file
        } else {
            cerr << "Failed to open chat_history.txt for saving.\n";  // Error handling if the file can't be opened
        }
    }
}

int main() {
    // Create the socket for communication
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;  // Exit if socket creation fails
    }

    // Set up the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);  // Use the defined port
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);  // Set server address to localhost

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;  // Exit if connection fails
    }

    cout << "Connected to the server.\n";

    // Start a new thread to receive messages from the server
    thread(receive_messages, sock).detach();

    string message;
    while (true) {
        getline(cin, message);  // Get user input for the message
        if (message == "exit") {
            close(sock);  // Close the socket if the user types "exit"
            break;
        }

        // Save the sent message to chat_history.txt
        save_sent_message(message);

        // Send the message to the server
        send(sock, message.c_str(), message.size(), 0);
    }

    return 0;
}

