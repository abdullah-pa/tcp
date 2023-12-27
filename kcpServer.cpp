#include <iostream>
#include <cstring>
#include "ikcp.h"
#include <chrono>
#include <thread>

uint32_t iclock() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main() {
    ikcpcb* kcp = ikcp_create(0x11223344, nullptr);
    kcp->output = [](const char* buf, int len, ikcpcb* /*kcp*/, void* user) -> int {
        std::cout << "Received from client: " << std::string(buf, len) << "\n";
    };
    ikcp_setmtu(kcp, 1400);
    uint32_t current = iclock();
    uint32_t slap = current + 20;
    uint32_t index = 0;
    while (true) {
        current = iclock();
        ikcp_update(kcp, current);
        if (current >= slap) {
            char message[100];
            snprintf(message, sizeof(message), "Hello, client! %d", index++);
            ikcp_send(kcp, message, static_cast<int>(strlen(message) + 1)); // Include the null terminator
            slap = current + 2000; // Update slap after sending the message
        }
        char buffer[2000];
        int len = sizeof(buffer);
        ikcp_input(kcp, buffer, len);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Use std::this_thread::sleep_for directly
    }
    ikcp_release(kcp);

    return 0;
}
