#include <iostream>
#include "ikcp.h"
#include <winsock2.h>
#include <chrono>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

// Define the server address and port
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 12345;

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create KCP object
    ikcpcb* kcp = ikcp_create(0x11223344, nullptr);

    // Set output callback function
    kcp->output = [](const char* buf, int len, ikcpcb* kcp, void* user) -> int {
        // Replace this with your actual send function
        // For example, if you're using TCP, you might use send()
        std::cout << "Client sending data: " << std::string(buf, len) << std::endl;
        return 0;
    };

    // Set KCP parameters
    ikcp_nodelay(kcp, 1, 10, 2, 1);

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to the server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);
    connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr));

    std::cout << "Connected to server" << std::endl;

    // Simulate receiving data from the server
    char recvBuffer[1024];
    std::string receivedData = "Hello, Client!";
    std::memcpy(recvBuffer, receivedData.c_str(), receivedData.size());

    // Pass the received data to KCP
    ikcp_input(kcp, recvBuffer, receivedData.size());

    // Update KCP (call this in your main loop)
    auto current = std::chrono::system_clock::now();
    uint32_t currentMs = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current.time_since_epoch()).count());
    ikcp_update(kcp, currentMs);

    // Send data using KCP (call this in your main loop)
    std::string sendData = "Hello, Server!";
    ikcp_send(kcp, sendData.c_str(), sendData.size());

    // Receive data using KCP (call this in your main loop)
    int len = ikcp_recv(kcp, recvBuffer, sizeof(recvBuffer));
    if (len > 0) {
        std::cout << "Client received data: " << std::string(recvBuffer, len) << std::endl;
    }

    // Release KCP object
    ikcp_release(kcp);

    // Close socket
    closesocket(clientSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
