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

#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#define RECIEVE 1
#define SEND 2

#define SHM_SIZE 1024  // size of shared memory
#define SHM_KEY 1234   // fixed key

char *formatfinder(char filetype[2]);

typedef struct {
    char filename[256];
    long filesize;
} header_t;

typedef struct {
    int socket;
} thread_arg_t;


void *receive_data (void *arg) {
    FILE *f;
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    header_t fileheader;
    int socket = thread_arg->socket;
    int data_received = 0, bytes_read = 0;
    int header_received = 0;
    char buffer[BUFFER_SIZE] = {0};

    printf("Receiving file header...\n");

    while ((header_received = recv(socket, &fileheader, sizeof(header_t), 0)) < sizeof(header_t)) {
        continue;
    }

    printf("Writing to %s...\n", fileheader.filename);

    /*
        Open file with given name and 
        write in chunks
    */
    f = fopen(fileheader.filename, "wb");
    if (f == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    while ((data_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        bytes_read += data_received;
        
        printf("\r%i/%ld bytes received", bytes_read, fileheader.filesize);

        fwrite(buffer, 1, data_received, f);
    }

    printf("\nFile received successfully from socket\n");

    close(socket);
    fclose(f);
}

int main() {
    int sockfd, socket_fd;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    pthread_t thread;
    thread_arg_t arg;

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

    // Accept connection
    if ((socket_fd = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection accepted from socket a\n");
    }

    /*----------------------------------------RECIEVE FILE------------------------------------------------*/

    arg.socket = socket_fd;

    printf("Creating thread...\n");

    // Create threads for each socket
    if (pthread_create(&thread, NULL, receive_data, &arg) != 0) {
        perror("Failed to create thread for socket_a");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish (optional, depending on your application's needs)
    pthread_join(thread, NULL);

    printf("Thread finished\n");

    close(sockfd);

    printf("Press Enter to exit...");
    getchar(); // Waits for the user to press Enter

    return 0;
}
