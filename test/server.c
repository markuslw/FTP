#include <stdio.h> // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h> // For exit()
#include <sys/socket.h> // For socket(), bind(), listen(), accept(), recv()
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> // For inet_pton()
#include <unistd.h> // For close()
#include <fcntl.h> // For file operations
#include <string.h> // For memset

#define PORT 8080
#define BUFFER_SIZE 1024

char *get_public_ip();

int main() {
    int sockfd, socket_a, socket_b, bytes_read;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};
    FILE *received_file, *file_to_send;
    int bytes_received = 0;

    // Convert the public IP address from string to binary form
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 2) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %s:%d\n", inet_ntoa(server_addr.sin_addr), PORT);  

    // Accept connection
    if ((socket_a = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted from socket a\n");

    // Receive file
    received_file = fopen("tmp.bin", "wb"); // Open a file for writing in binary mode
    if (received_file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    while ((bytes_received = recv(socket_a, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, received_file);
    }
    if (bytes_received < 0) {
        perror("Recv failed from socket a");
        exit(EXIT_FAILURE);
    }

    char sign[2];
    FILE *tmp_file = fopen("tmp.bin", "rb");
    fread(sign, 1, 2, tmp_file);
    fclose(tmp_file);

    if ((sign[0] & 0xff) == 0xff && (sign[1] & 0xff) == 0xd8) {
        printf("JPEG\n");
    } else {
        printf("idk\n");
    }

    printf("File received successfully from socket a\n");
    close(socket_a);
    fclose(received_file);

    close(sockfd);

    return 0;
}
