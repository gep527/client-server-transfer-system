#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include "../include/pack109.hpp"
#include "../include/HashMap.hpp"

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;

using File = std::vector<u8>;  // shorthand for a byte vector

// --- Utility Functions ---

// Encrypts/decrypts a byte vector using XOR key (42)
// Since XOR is symmetric, the same function can decrypt
File encrypt(const File &data) {
    File result;
    for (u8 b : data) {
        result.push_back(b ^ 42);
    }
    return result;
}

File decrypt(const File &data) {
    return encrypt(data); // XOR by 42 is reversible
}

// Reads the entire contents of a file into a vector of bytes
vector<u8> read_file_bytes(const string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        perror("fopen failed");
        exit(1);
    }
    return vector<u8>((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
}

// --- Main Client ---
int main(int argc, char *argv[]) {
    // Check correct usage
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Parse arguments
    int portno = atoi(argv[2]);

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Resolve server host
    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // Set up server address struct
    struct sockaddr_in serv_addr {};
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    bool comm = true;
    while (comm) {
        // Ask user what they want to do
        cout << "Do you want to 'request', 'send' a File, or 'quit'? ";
        string response;
        std::getline(cin, response);

        if (response == "quit") {
            cout << "Goodbye!" << endl;
            comm = false;
            break;
        }

        // Ask user for file name
        cout << "What file? ";
        string file_name;
        std::getline(cin, file_name);
        string file_path = "./files/" + file_name;

        File message; // will hold the serialized+encrypted message

        if (response == "request") {
            // Build request struct and serialize
            struct Request req = { .name = file_name };
            File ser = pack109::serialize(req);

            // Encrypt message before sending
            message = encrypt(ser);

        } else if (response == "send") {
            // Read file contents into byte vector
            vector<u8> file_bytes = read_file_bytes(file_path);

            // Build file struct and serialize
            struct FileStruct f = { .name = file_name, .bytes = file_bytes };
            File ser = pack109::serialize(f);

            // Encrypt message before sending
            message = encrypt(ser);

        } else {
            cout << "Invalid option" << endl;
            continue;
        }

        // --- Send message to server ---
        int n = write(sockfd, message.data(), message.size());
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }

        // --- Read response from server ---
        char buffer[65536];
        bzero(buffer, sizeof(buffer));
        n = read(sockfd, buffer, sizeof(buffer) - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        // Convert raw buffer to vector of bytes
        File recv_bytes(buffer, buffer + n);

        // Decrypt response
        File decrypted = decrypt(recv_bytes);

        // Deserialize based on message type
        int size_key = decrypted[3];
        if (size_key == 4) {
            // Response is a file
            struct FileStruct file_deser = pack109::deserialize_file(decrypted);
            cout << file_deser.name << " contents are:" << endl;
            for (char c : file_deser.bytes) cout << c;
            cout << endl;

        } else if (size_key == 6) {
            // Response is a status message
            struct Status status_deser = pack109::deserialize_status(decrypted);
            cout << status_deser.message << endl;
        }
    }

    // Close socket before exiting
    close(sockfd);
    return 0;
}
