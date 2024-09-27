#include <iostream>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <sys/socket.h>
#include <cstring> // For memset() and bzero()
#include <cstdlib> // For exit()

using namespace std;

int main() {
    const char *ip = "127.0.0.1"; // Server IP
    const int port = 5566; // Server port

    int sock;
    struct sockaddr_in addr;
    char buffer[1024];

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM for UDP
    if (sock < 0) {
        cerr << "[-] UDP Socket error" << endl;
        exit(1);
    }
    cout << "[+] UDP client socket created." << endl;

    // Prepare server address
    memset(&addr, 0, sizeof(addr)); // Set all bytes to 0
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Convert port to network byte order
    addr.sin_addr.s_addr = inet_addr(ip);

    // Send a message to the server
    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "HELLO, THIS IS UDP CLIENT.");
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
    cout << "Client: " << buffer << endl;

    // Receive a message from the server
    bzero(buffer, sizeof(buffer));
    recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);
    cout << "Server: " << buffer << endl;

    // Close the socket
    close(sock);
    cout << "Disconnected from the server" << endl;

    return 0;
}
