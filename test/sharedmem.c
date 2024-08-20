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
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#define SHM_SIZE 1024  // size of shared memory
#define SHM_KEY 1234   // fixed key

//char *get_public_ip();

int main() {
    clock_t start, end;
    double elapsed;
    
    start = clock();
    
    char buffer[BUFFER_SIZE] = {0xff, 0xd8};

    // find signature in two first bytes
    unsigned char filetype[2];
    for (int i = 0; i < 2; i++) {
        unsigned char mask = buffer[i] & 0xff;
        filetype[i] = mask;
    };

    // create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // attach to shared memory
    char *data_write = (char*) shmat(shmid, NULL, 0);
    if (data_write == (char*) -1) {
        perror("shmat");
        return 1;
    }

    // write data to shared memory
    data_write[0] = filetype[0];
    data_write[1] = filetype[1];
    printf("[C]\t Data written to shared memory: %02x %02x\n", filetype[0], filetype[1]);

    system("python3 filesignature.py");

    // attach to shared memory
    char *data_read = (char*) shmat(shmid, NULL, 0);
    if (data_read == (char*) -1) {
        perror("shmat");
        return 1;
    }

    // read data from shared memory
    printf("[C]\t Data read from shared memory: %s\n", data_read);

    // detach from shared memory
    if (shmdt(data_read) == -1) {
        perror("shmdt");
        return 1;
    }

    // remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        return 1;
    }

    end = clock();

    elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f seconds\n", elapsed);

    return 0;
}
