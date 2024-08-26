#include <stdio.h>      // For printf(), perror(), fgets(), popen(), pclose()
#include <stdlib.h>     // For exit()
#include <string.h>

typedef struct {
    char filename[256];
    char fileformat[8];
} header_t;

int main() {

    header_t dog;
    strcpy(dog.filename, "image.png");
    strcpy(dog.fileformat, "123456789");

    printf("%li\n", sizeof(dog));

    printf("%li\n", sizeof(header_t));

    return 0;
}
