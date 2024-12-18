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

#define PORT 8080

std::mutex chat_history_mutex;  // Mutex for chat history file

void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "Disconnected from server.\n";
            close(sock);
            break;
        }
        std::cout << "Received: " << buffer << std::endl;

        // Save received messages to chat_history.txt
        {
            std::lock_guard<std::mutex> lock(chat_history_mutex);
            std::ofstream out("chat_history.txt", std::ios::app);
            if (out.is_open()) {
                out << "Received: " << buffer << std::endl;
            } else {
                std::cerr << "Failed to open chat_history.txt for saving.\n";
            }
        }
    }
}

void save_sent_message(const std::string& message) {
    // Save sent messages to chat_history.txt
    {
        std::lock_guard<std::mutex> lock(chat_history_mutex);
        std::ofstream out("chat_history.txt", std::ios::app);
        if (out.is_open()) {
            out << "Sent: " << message << std::endl;
        } else {
            std::cerr << "Failed to open chat_history.txt for saving.\n";
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    std::cout << "Connected to the server.\n";

    // Start the message receiving thread
    std::thread(receive_messages, sock).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            close(sock);
            break;
        }

        // Save the sent message to chat_history.txt
        save_sent_message(message);

        // Send the message to the server
        send(sock, message.c_str(), message.size(), 0);
    }

    return 0;
}
