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

    char *fileformat;
    switch (filetype[0]) {
        case 0xff:
            switch (filetype[1]) {
                case 0xd8:
                    fileformat = ".jpg";
                    break;
                case 0xfb:
                    fileformat = ".mp3";
                    break;
                default:
                    break;
            }
            break;
        case 0x25:
            switch (filetype[1]) {
                case 0x50:
                    fileformat = ".pdf";
                    break;
                default:
                    break;
            }
            break;
        case 0x50: // For .png files
            switch (filetype[1]) {
                case 0x4e:
                    fileformat = ".png";
                    break;
                default:
                    break;
            }
            break;
        case 0x4d: // For .mov files
            switch (filetype[1]) {
                case 0x4f:
                    fileformat = ".mov";
                    break;
                default:
                    break;
            }
            break;
        case 0x55: // For .zip files
            switch (filetype[1]) {
                case 0x0a:
                    fileformat = ".zip";
                    break;
                default:
                    break;
            }
            break;
        case 0x37: // For .7z files
            switch (filetype[1]) {
                case 0x7a:
                    fileformat = ".7z";
                    break;
                default:
                    break;
            }
            break;
        case 0x66: // For .mp4 files
            switch (filetype[1]) {
                case 0x74:
                    fileformat = ".mp4";
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    printf("%s\n", fileformat);

    end = clock();

    elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f seconds\n", elapsed);

    return 0;
}
