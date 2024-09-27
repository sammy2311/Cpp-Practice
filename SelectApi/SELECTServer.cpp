#include <iostream>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <sys/socket.h>
#include <cstring> // For memset() and bzero()
#include <cstdlib> // For exit()
#include <sys/select.h> // For select()

using namespace std;

int main() {
    const char *ip = "127.0.0.1"; // Server IP
    const int port = 5566; // Server port

    int server_sock, client_sock;
    int udp_sock;
    struct sockaddr_in server_addr, client_addr, udp_addr; //sockaddr_in is a structure used to specify an internet socket address. => bind(),connect() and accept()
    socklen_t addr_size;//socklen_t is a data type used for the size of socket-related structures =>accept(),bind() and getsockname()
    char buffer[1024];

    fd_set read_fds;
    int max_fd; // Maximum number of descriptors ready.
    int activity;

    // Create TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM for TCP
    if (server_sock < 0) {
        cerr << "[-] TCP Socket error" << endl;
        exit(1);
    }
    cout << "[+] TCP server socket created." << endl;

    // Create UDP socket
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM for UDP
    if (udp_sock < 0) {
        cerr << "[-] UDP Socket error" << endl;
        exit(1);
    }
    cout << "[+] UDP server socket created." << endl;

    // Prepare server address
    memset(&server_addr, 0, sizeof(server_addr)); // Set all bytes to 0
    server_addr.sin_family = AF_INET; //sa_family_t => Address family (IPV4)
    server_addr.sin_port = htons(port); // Convert port to network byte order hons =>HOST TO NETWORK SHORT
    server_addr.sin_addr.s_addr = inet_addr(ip); //IP Address

    // Bind TCP socket to address
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "[-] Bind error" << endl;
        close(server_sock);
        close(udp_sock);
        exit(1);
    }
    cout << "[+] Bind to port " << port << endl;

    // Bind UDP socket to address
    if (bind(udp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "[-] Bind UDP error" << endl;
        close(server_sock);
        close(udp_sock);
        exit(1);
    }

    // Listen for TCP connections
    if (listen(server_sock, 5) < 0) {
        cerr << "[-] Listen error" << endl;
        close(server_sock);
        close(udp_sock);
        exit(1);
    }
    cout << "Listening for TCP and UDP connections..." << endl;

    // Main server loop
    while (true) {
        FD_ZERO(&read_fds);//Initializes the set to be zero
        FD_SET(server_sock, &read_fds);//FD_SET Declares a set of file descriptors to be monitored by select() for readability.
        FD_SET(udp_sock, &read_fds);
        max_fd = max(server_sock, udp_sock) + 1;//As select() goes from max_fd -1 so we add +1.

        // Use select() to monitor sockets
        
        activity = select(max_fd, &read_fds, nullptr, nullptr, nullptr); 
        //select() monitors the TCP and UDP sockets to detect when 
        //they are ready for reading, allowing the server to handle incoming connections or data from clients.

        if (activity < 0) {
            cerr << "[-] Select error" << endl;
            close(server_sock);
            close(udp_sock);
            exit(1);
        }

        // Check if there is an incoming TCP connection
        if (FD_ISSET(server_sock, &read_fds)) {
            addr_size = sizeof(client_addr);
            client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
            if (client_sock < 0) {
                cerr << "[-] Accept error" << endl;
                close(server_sock);
                close(udp_sock);
                exit(1);
            }
            cout << "TCP Client connected" << endl;

            // Communicate with the TCP client
            bzero(buffer, sizeof(buffer));
            recv(client_sock, buffer, sizeof(buffer), 0);
            cout << "Client: " << buffer << endl;

            // Send a message back to the TCP client
            bzero(buffer, sizeof(buffer));
            strcpy(buffer, "HI, THIS IS THE SERVER. HAVE A NICE DAY");
            send(client_sock, buffer, strlen(buffer), 0);

            // Close the TCP client socket
            close(client_sock);
            cout << "TCP Client Disconnected" << endl;
        }

        // Check if there is incoming UDP data
        if (FD_ISSET(udp_sock, &read_fds)) {
            bzero(buffer, sizeof(buffer));
            addr_size = sizeof(udp_addr);
            recvfrom(udp_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&udp_addr, &addr_size);
            cout << "UDP Client: " << buffer << endl;

            // Send a response back to the UDP client
            bzero(buffer, sizeof(buffer));
            strcpy(buffer, "HI, THIS IS THE SERVER. HAVE A NICE DAY");
            sendto(udp_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&udp_addr, sizeof(udp_addr));
        }
    }

    // Close sockets (this code will never be reached in this example)
    close(server_sock);
    close(udp_sock);

    return 0;
}
