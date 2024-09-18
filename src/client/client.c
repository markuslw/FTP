#include <stdio.h>      // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h>     // For exit()
#ifdef _WIN32
#include <winsock2.h>   // For socket functions
#include <ws2tcpip.h>   // For inet_pton()
#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib
#else
#include <sys/socket.h> // For socket(), connect(), send()
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>  // For inet_pton()
#include <unistd.h>     // For close()
#endif
#include <fcntl.h>      // For file operations
#include <string.h>     // For memset and strcpy

#define PORT 8080
#define BUFFER_SIZE 1024

#define RECIEVE 1
#define SEND 2

typedef struct {
    char filename[256];
    long filesize;
} header_t;

int main() {
    int sockfd, bytes_received, bytes_read, bytes_sent = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    FILE *f;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }
#endif

    /*--------------------------------------CREATE SOCKETS-------------------------------------------------*/

    // Set address
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        perror("Invalid address");
        getchar();
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        getchar();
        exit(EXIT_FAILURE);
    }

    /*------------------------------------ESTABLISH CONNECTIONS----------------------------------------------*/
    
    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    printf("Connecting to server...\n");
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        getchar();
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    /*----------------------------------------SEND FILE------------------------------------------------*/

    char filepath[256] = "image.png";
    //printf("Enter filepath (./Downloads/my_report.docx): ");
    //scanf("%255s", filepath);
    int filepath_length = strlen(filepath);

    // Open file to send
    f = fopen(filepath, "rb");
    if (f == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    } else {
        printf("File opened successfully\n");
    }

    // Determine the size of the file
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Freaky algorithm to get filename from filepath
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
    filename[j] = '\0';

    printf("Sending file header for %s...\n", filename);

    /*
        Send miscellaneous data about the file to the server
    */
    header_t fileheader;
    strcpy(fileheader.filename, filename);
    fileheader.filesize = filesize;
    send(sockfd, &fileheader, sizeof(fileheader), 0);

    printf("Sending file %s...\n", filename);

    /*
        Read and send file in chunks
    */
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        if (send(sockfd, buffer, bytes_read, 0) != bytes_read) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
        bytes_sent += bytes_read;
        printf("\r%i bytes sent", bytes_sent);
    }

    printf("\nFile sent successfully\n");

    fclose(f);
    close(sockfd);

#ifdef _WIN32
    WSACleanup();  // Cleanup Winsock when done
#endif

    printf("Press Enter to exit...");
    getchar(); // Waits for the user to press Enter
    
    return 0;
}
