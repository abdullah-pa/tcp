#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include "ikcp.h"
#include <winsock2.h>
#include <chrono>
#include <cstring>



// Define the port number for the server
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
        std::cout << "Server sending data: " << std::string(buf, len) << std::endl;
        return 0;
    };

    // Set KCP parameters
    ikcp_nodelay(kcp, 1, 10, 2, 1);

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to a specific port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);
    bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr));

    // Listen for incoming connections
    listen(serverSocket, 1);

    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

    // Accept a client connection
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);

    std::cout << "Client connected" << std::endl;

    // Simulate receiving data from the client
    char recvBuffer[1024];
    std::string receivedData = "Hello, Server!";
    std::memcpy(recvBuffer, receivedData.c_str(), receivedData.size());

    // Pass the received data to KCP
    ikcp_input(kcp, recvBuffer, receivedData.size());

    // Update KCP (call this in your main loop)
    auto current = std::chrono::system_clock::now();
    uint32_t currentMs = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current.time_since_epoch()).count());
    ikcp_update(kcp, currentMs);

    // Send data using KCP (call this in your main loop)
    std::string sendData = "Hello, Client!";
    ikcp_send(kcp, sendData.c_str(), sendData.size());

    // Receive data using KCP (call this in your main loop)
    int len = ikcp_recv(kcp, recvBuffer, sizeof(recvBuffer));
    if (len > 0) {
        std::cout << "Server received data: " << std::string(recvBuffer, len) << std::endl;
    }

    // Release KCP object
    ikcp_release(kcp);

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
