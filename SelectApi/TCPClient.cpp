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
    char buffer[1024]; // Added buffer variable

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (sock < 0) {
        cerr << "[-] Socket error" << endl;
        exit(1);
    }
    cout << "[+] TCP client socket created." << endl;

    // Prepare server address
    memset(&addr, 0, sizeof(addr)); // Set all bytes to 0
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Convert port to network byte order
    addr.sin_addr.s_addr = inet_addr(ip);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "[-] Connection error" << endl;
        close(sock); // Close socket on error
        exit(1);
    }
    cout << "[+] Connected to the Server" << endl;

    // Send a message to the server
    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "HELLO, THIS IS CLIENT.");
    cout << "Client: " << buffer << endl;
    send(sock, buffer, strlen(buffer), 0);

    // Receive a message from the server
    bzero(buffer, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    cout << "Server: " << buffer << endl;

    // Close the socket
    close(sock);
    cout << "Disconnected from the server" << endl;

    return 0;
}
