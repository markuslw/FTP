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
    char fileformat[8];
} header_t;

typedef struct {
    int connection_type;
    char dest_addr[INET_ADDRSTRLEN];
    char src_addr[INET_ADDRSTRLEN];
} connection_t;

typedef struct {
    int socket_self;
    int socket_other;
} thread_arg_t;


void *receive_data (void *arg) {
    FILE *f;
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    header_t fileheader;
    
    int socket_self = thread_arg->socket_self;
    int socket_other = thread_arg->socket_other;
    int bytes_received = 0, bytes_read = 0;
    char buffer[BUFFER_SIZE] = {0};

    // Receive file header struct
    recv(socket_self, &fileheader, sizeof(header_t), 0);

    // Receive file and write
    f = fopen(fileheader.filename, "wb"); // Open a file for writing in binary mode
    if (f == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    while ((bytes_received = recv(socket_self, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, f);
    }
    if (bytes_received < 0) {
        perror("Recv failed from self");
        exit(EXIT_FAILURE);
    }
    printf("File received successfully from self\n");
    close(socket_self);

    fseek(f, 0, SEEK_SET);

    // Send to other
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        if (send(socket_other, buffer, bytes_read, 0) != bytes_read) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
    if (bytes_read < 0) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

    // Delete the file while it's still open
    if (remove(fileheader.filename) != 0) {
        perror("Failed to delete file");
        fclose(f);
        return 1;
    }

    close(socket_other);

    // still accessible until fclose is called
    fclose(f);
}

int main() {
    int sockfd, socket_a, socket_b, bytes_read;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = {0};
    char client_ip[INET_ADDRSTRLEN];
    unsigned char filetype[2];
    FILE *received_file, *file_to_send;
    int bytes_received = 0;
    pthread_t thread_a, thread_b;
    thread_arg_t arg_a, arg_b;

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
    } else {
        printf("Connection accepted from socket a\n");
        connection_t connection_a;
        recv(socket_a, &connection_a, sizeof(connection_t), 0);
    }

    //Accept connection B
    if ((socket_b = accept(sockfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    } else {
        printf("Connection accepted from socket b\n");
        connection_t connection_b;
        recv(socket_b, &connection_b, sizeof(connection_t), 0);
    }
    

    /*----------------------------------------RECIEVE FILE------------------------------------------------*/

    arg_a.socket_self = socket_a;
    arg_a.socket_other = socket_b;

    arg_b.socket_self = socket_b;
    arg_b.socket_other = socket_a;

    // Create threads for each socket
    if (pthread_create(&thread_a, NULL, receive_data, &arg_a) != 0) {
        perror("Failed to create thread for socket_a");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread_b, NULL, receive_data, &arg_b) != 0) {
        perror("Failed to create thread for socket_b");
        exit(EXIT_FAILURE);
    }

    // Wait for the threads to finish (optional, depending on your application's needs)
    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);

    close(sockfd);

    return 0;
}
