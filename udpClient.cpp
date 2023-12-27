#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return -1;
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket\n";
        WSACleanup();
        return -1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8080);                
    const char* message = "Hello, UDP Server!";
    int bytesSent = sendto(clientSocket, message, strlen(message), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Error sending data\n";
    } else {
        std::cout << "Sent to server: " << message << "\n";
    }
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
