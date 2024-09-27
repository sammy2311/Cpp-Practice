#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <fstream>

using namespace std;

void uploadFile(int sock, const string& filename) {
    ifstream inFile(filename, ios::binary);

    if (!inFile.is_open()) {
        cerr << "Error opening file for reading: " << filename << endl;
        return;
    }

    // Send the upload command
    string command = "UPLOAD " + filename;
    send(sock, command.c_str(), command.size(), 0);

    // Send the file data
    char buffer[1024];
    while (!inFile.eof()) {
        inFile.read(buffer, 1024);
        send(sock, buffer, inFile.gcount(), 0);
    }

    inFile.close();
    cout << "File " << filename << " uploaded successfully." << endl;
}

void downloadFile(int sock, const string& filename) {
    // Send the download command
    string command = "DOWNLOAD " + filename;
    send(sock, command.c_str(), command.size(), 0);

    // Receive the file data
    ofstream outFile(filename, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(sock, buffer, 1024, 0)) > 0) {
        outFile.write(buffer, bytesReceived);
    }

    outFile.close();
    cout << "File " << filename << " downloaded successfully." << endl;
}

int main() {
    const char *ip = "127.0.0.1";
    const int port =  5566;
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "[-] Error in socket" << endl;
        exit(1);
    }
    cout << "[+]Server socket created" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "[-]Error in connecting" << endl;
        exit(1);
    }
    cout << "[+]Connected to server" << endl;

    // Choose to upload or download
    int choice;
    string filename;
    cout << "Enter 1 to upload a file, 2 to download a file: ";
    cin >> choice;
    cout << "Enter the filename: ";
    cin >> filename;

    if (choice == 1) {
        uploadFile(sock, filename);
    } else if (choice == 2) {
        downloadFile(sock, filename);
    } else {
        cerr << "Invalid choice" << endl;
    }

    close(sock);
    return 0;
}
