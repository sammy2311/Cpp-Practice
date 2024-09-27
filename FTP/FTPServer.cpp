#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <fstream>

using namespace std;

void handleClient(int client_sock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    
    // Receive command
    recv(client_sock, buffer, sizeof(buffer), 0);
    string command(buffer);

    if (command.find("UPLOAD") == 0) {
        // Extract filename from the command
        string filename = command.substr(7); // "UPLOAD " is 7 characters
        ofstream outFile(filename, ios::binary);
        
        if (!outFile.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        // Receive file data
        int bytesReceived;
        while ((bytesReceived = recv(client_sock, buffer, 1024, 0)) > 0) {
            outFile.write(buffer, bytesReceived);
        }

        outFile.close();
        cout << "File " << filename << " received and saved." << endl;

    } else if (command.find("DOWNLOAD") == 0) {
        // Extract filename from the command
        string filename = command.substr(9); // "DOWNLOAD " is 9 characters
        ifstream inFile(filename, ios::binary);

        if (!inFile.is_open()) {
            cerr << "File not found: " << filename << endl;
            return;
        }

        // Send file data
        while (!inFile.eof()) {
            inFile.read(buffer, 1024);
            send(client_sock, buffer, inFile.gcount(), 0);
        }

        inFile.close();
        cout << "File " << filename << " sent to client." << endl;
    }

    close(client_sock);
}

int main() {
    const char *ip = "127.0.0.1";
    const int port =  5566;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr << "[-] Error in socket" << endl;
        exit(1);
    }
    cout << "[+]Server socket created" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "[-] Error in Binding" << endl;
        exit(1);
    }
    cout << "[+]Binding Successful" << endl;

    if (listen(server_sock, 10) == 0) {
        cout << "[+]Listening..." << endl;
    } else {
        cerr << "[-] Error in Listening" << endl;
        exit(1);
    }

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

    if (client_sock < 0) {
        cerr << "[-] Error in Accepting Connection" << endl;
        exit(1);
    }

    handleClient(client_sock);
    close(server_sock);

    return 0;
}
