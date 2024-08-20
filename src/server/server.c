#include <stdio.h> // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h> // For exit()
#include <sys/socket.h> // For socket(), bind(), listen(), accept(), recv()
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> // For inet_pton()
#include <unistd.h> // For close()
#include <fcntl.h> // For file operations
#include <string.h> // For memset

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#define SHM_SIZE 1024  // size of shared memory
#define SHM_KEY 1234   // fixed key

char *formatfinder(char filetype[2]);

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
    int sockfd, socket_a, socket_b, bytes_read;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};
    char client_ip[INET_ADDRSTRLEN];
    unsigned char filetype[2];
    FILE *received_file, *file_to_send;
    int bytes_received = 0;

    /*--------------------------------------CREATE SOCKETS-------------------------------------------------*/

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

    /*------------------------------------ACCEPT CONNECTIONS----------------------------------------------*/

    // Accept connection A
    if ((socket_a = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted from socket a\n");

    connection_t connection_a;
    recv(socket_a, &connection_a, sizeof(connection_t), 0);

    //Accept connection B
    if ((socket_b = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted from socket b\n");
    
    connection_t connection_b;
    recv(socket_b, &connection_b, sizeof(connection_t), 0);

    /*----------------------------------------RECIEVE FILE------------------------------------------------*/

    header_t fileheader;
    recv(socket_a, &fileheader, sizeof(header_t), 0);
    char *filename = fileheader.filename;
    strcat(filename, fileheader.fileformat);

    // Receive file, write file and close socket
    received_file = fopen(filename, "wb"); // Open a file for writing in binary mode
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
    printf("File received successfully from socket a\n");
    close(socket_a);



    fclose(received_file);

    close(sockfd);

    return 0;
}
