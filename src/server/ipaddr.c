#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESPONSE_SIZE 100

char *get_public_ip() {
    FILE *fp;
    char *public_ip = malloc(MAX_RESPONSE_SIZE * sizeof(char));

    fp = popen("curl -s4 ifconfig.co", "r");
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    if (fgets(public_ip, MAX_RESPONSE_SIZE, fp) == NULL) {
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    pclose(fp);

    return public_ip;
}