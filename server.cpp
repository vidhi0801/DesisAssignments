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

#define PORT 8080

std::vector<int> clients;
std::vector<std::string> chat_history;
std::mutex clients_mutex;
std::mutex chat_mutex;

void broadcast_message(const std::string& message, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_fd : clients) {
        if (client_fd != sender_fd) {
            size_t total_sent = 0;
            size_t to_send = message.size();
            const char* msg = message.c_str();
            while (total_sent < to_send) {
                int sent = send(client_fd, msg + total_sent, to_send - total_sent, 0);
                if (sent <= 0) {
                    break;
                }
                total_sent += sent;
            }
        }
    }
}

void handle_client(int client_fd) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
            std::cout << "Client " << client_fd << " disconnected.\n";
            break;
        }
        std::string message = "Client " + std::to_string(client_fd) + ": " + buffer + "\n";
        std::cout << message;
        broadcast_message(message, client_fd);

        // Save chat history
        {
            std::lock_guard<std::mutex> lock(chat_mutex);
            chat_history.push_back(message);
        }
    }
}

void save_chat_history() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(30));  // Save every 30 seconds
        std::ofstream out("chat_history.txt", std::ios::app);
        if (out.is_open()) {
            std::lock_guard<std::mutex> lock(chat_mutex);
            for (const std::string& message : chat_history) {
                out << message;
            }
            std::cout << "Chat history saved.\n";
            chat_history.clear();  // Clear after saving
        } else {
            std::cerr << "Failed to open chat_history.txt for saving.\n";
        }
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Start the chat history saving thread
    std::thread(save_chat_history).detach();

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        std::cout << "New client connected: " << client_fd << std::endl;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
        }

        std::thread(handle_client, client_fd).detach();
    }

    close(server_fd);
    return 0;
}


