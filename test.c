#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
    const char *msg = "hello world";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256((const unsigned char *)msg, strlen(msg), hash);

    printf("SHA256: ");
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        printf("%02x", hash[i]);
    printf("\n");

    return 0;
}