#include <stdio.h> // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h> // For exit()
#include <sys/socket.h> // For socket(), connect(), send()
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> // For inet_pton()
#include <unistd.h> // For close()
#include <fcntl.h> // For file operations
#include <string.h> // For memset and strcpy

#define PORT 8080
#define BUFFER_SIZE 1024

#define RECIEVE 1
#define SEND 2

typedef struct {
    char filename[256];
    char fileformat[8];
} header_t;

typedef struct {
    int connection_type;
    char dest_addr[INET_ADDRSTRLEN];
    char src_addr[INET_ADDRSTRLEN];
} connection_t;

int main() {
    int sockfd, bytes_received;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    FILE *file_to_send, *received_file;
    ssize_t bytes_read;

    /*--------------------------------------CREATE SOCKETS-------------------------------------------------*/

    // Convert the public IP address from string to binary form
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    /*------------------------------------ESTABLISH CONNECTIONS----------------------------------------------*/
    
    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    printf("Connecting to: %s\n", inet_ntoa(server_addr.sin_addr));
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    connection_t connection;
    int connection_type;
    char dest_addr[32];

    printf("Enter connection type\n\t1. Recieve\n\t2. Send\n\n");
    scanf("%d", &connection_type);
    strcpy(connection.connection_type, connection_type);

    printf("Enter the public IPv4 destination address: ");
    scanf("%31s", dest_addr);
    strcpy(connection.dest_addr, dest_addr);

    send(sockfd, &connection, sizeof(connection), 0);

    /*----------------------------------------SEND FILE------------------------------------------------*/

    char filepath[256];
    printf("Enter filepath (/Downloads/my_report.docx): ");
    scanf("%255s", filepath);
    int filepath_length = strlen(filepath);

    // Open file to send
    file_to_send = fopen(filepath, "rb"); // Open file for reading in binary mode
    if (file_to_send == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char filename[256];
    for (int i = 0; i < filepath_length; i++) {
        if (filepath[i] == '/' || filepath[i] == '\\') {
            i = 0;
        } else if (filepath[i] == '\0') {
            filename[i] = '\0';
            break;
        }
        filename[i] = filepath[i];
    }

    header_t fileheader;
    strcpy(fileheader.filename, filename);

    send(sockfd, &fileheader, sizeof(fileheader), 0);

    // Read from file and send over socket
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file_to_send)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) != bytes_read) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
    if (bytes_read < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    printf("File sent successfully\n");
    fclose(file_to_send);
    close(sockfd);
    
    return 0;
}
