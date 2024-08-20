#include <stdio.h> // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h> // For exit()
#include <fcntl.h> // For file operations
#include <string.h> // For memset

#define BUFFER_SIZE 1024

int main() {
    FILE *f = fopen("pic.JPG", "rb");
    size_t bytes_read;
    char buffer[2];
    fread(buffer, 1, 2, f);
    fclose(f);

    unsigned char filetype[2];

    for (int i = 0; i < 2; i++) {
        unsigned char mask = buffer[i] & 0xff;
        printf("%x\n", mask);
        filetype[i] = mask;
    };

    if (filetype[0] == 0xff && filetype[1] == 0xd8) {
        printf("JPEG\n");
    }
    printf("%x")
}