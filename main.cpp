#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

bool connectSocket(int sockfd, const sockaddr* addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) == -1) {
        std::cerr << "connect() failed: " << std::strerror(errno) << '\n';
        return false;
    }
    return true;
}

}

int main() {
    // Create TCP socket (IPv4, stream-oriented)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "socket() failed: " << std::strerror(errno) << '\n';
        return EXIT_FAILURE;
    }

    // Configure server address and port
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8111);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1) {
        std::cerr << "Invalid IP address\n";
        close(sock);
        return EXIT_FAILURE;
    }

    // Connect to the server
    if (!connectSocket(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr))) {
        close(sock);
        return EXIT_FAILURE;
    }

    // Send data to the server
    const char* token = "TEST";
    ssize_t sent = send(sock, token, std::strlen(token), 0);
    if (sent == -1) {
        std::cerr << "send() failed: " << std::strerror(errno) << '\n';
        close(sock);
        return EXIT_FAILURE;
    }

    // Receive response from the server
    char buffer[1024]{};
    ssize_t received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (received < 0) {
        std::cerr << "recv() failed: " << std::strerror(errno) << '\n';
        close(sock);
        return EXIT_FAILURE;
    }

    if (received == 0) {
        std::cout << "Server closed connection\n";
    } else {
        buffer[received] = '\0';
        std::cout << buffer << '\n';
    }

    close(sock);
    return EXIT_SUCCESS;
}
