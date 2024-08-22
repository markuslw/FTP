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

int main() {
    int sockfd, bytes_received, bytes_read;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    FILE *f;

    /*--------------------------------------CREATE SOCKETS-------------------------------------------------*/

    // Convert the public IP address from string to binary form
    server_addr.sin_addr.s_addr = inet_addr("10.0.0.26");
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

    /*----------------------------------------SEND FILE------------------------------------------------*/

    char filepath[256] = "image.png";
    //printf("Enter filepath (./Downloads/my_report.docx): ");
    //scanf("%255s", filepath);
    int filepath_length = strlen(filepath);

    // Open file to send
    f = fopen(filepath, "rb"); // Open file for reading in binary mode
    if (f == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    } else {
        printf("File opened successfully\n");
    }

    char filename[256];
    int j = 0;
    for (int i = 0; i < filepath_length; i++) {
        if ((filepath[i] == '.' && filepath[i+1] == '/') || (filepath[i] == '.' && filepath[i+1] == '\\')) {
            i += 1;
            continue;
        } else if (filepath[i] == '/' || filepath[i] == '\\') {
            j = 0;
            continue;
        } else if (filepath[i] == '\0') {
            filename[j] = '\0';
            break;
        }
        filename[j] = filepath[i];
        j++;
    }

    printf("Sending file header for %s...\n", filename);

    header_t fileheader;
    strcpy(fileheader.filename, filename);
    send(sockfd, &fileheader, sizeof(fileheader), 0);

    printf("Sending file %s...\n", filename);

    // Read from file and send over socket
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
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
    
    fclose(f);
    close(sockfd);
    
    return 0;
}
