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
    FILE *f;

    // find signature in two first bytes
    unsigned char filetype[2];
    for (int i = 0; i < 2; i++) {
        unsigned char mask = buffer[i] & 0xff;
        filetype[i] = mask;
    };

    f = fopen("shared.bin", "wb");
    
    fwrite(filetype, sizeof(unsigned char), 2, f);

    fclose(f);

    system("python3 filesignature_storage.py");

    f = fopen("shared.bin", "r");

    char buf[5];

    fread(buf, sizeof(char), 4, f);

    buf[5] = '\0';

    printf("\n[C] Read from shared file: [%s]\n", buf);

    fclose(f);

    end = clock();

    elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f seconds\n", elapsed);

    return 0;
}
