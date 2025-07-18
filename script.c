#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>

void OAuth();

void handle_client(int sock, struct sockaddr_in* client_addr) {
    fcntl(sock, F_SETFL, O_NONBLOCK);

    int alive = 1;
    while(alive) {
        fd_set read_fds;
        struct timeval timeout;

        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);

        timeout.tv_sec = 20;
        timeout.tv_usec = 0;

        int ready = select(sock + 1, &read_fds, NULL, NULL, &timeout);
        if(ready <= 0) break;

        int buffer_len = 1024;
        char buffer[buffer_len];
        if(recv(sock, buffer, buffer_len, 0) <= 0) break;

        char* token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        printf("filename: %s\n", token);

        if(strcmp(token, "/OAuth2") == 0) {
            
        } 
        else if()

    }
}

int main() {
    int PORT = 8080;
    char* HOST = "localhost";

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    int listen_sock;
    if((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket failed\n");
        exit(1);
    }

    if(bind(listen_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("bind failed\n");
        exit(1);
    }

    if(listen(listen_sock, 5) < 0) {
        printf("listen failed\n");
        exit(1);
    }

    printf("waiting for incoming connections\n");
    while(1) {
        socklen_t client_len = sizeof(struct sockaddr_in);
        struct sockaddr_in* client_addr = malloc(client_len);

        int sock;
        if((sock = accept(listen_sock, (struct sockaddr*) client_addr, &client_len)) < 0) {
            printf("unable to accept connection\n");
            exit(1);
        }

        handle_client(sock, client_addr);
        free(client_addr);
    }

    return 0;
}

void OAuth() {
    char* client_id = "55593084363-i1897onpemdnsk3q79m14t1f402e3vri.apps.googleusercontent.com";
    char* project_id = "oauth-study-465305";
    char* auth_uri = "https://accounts.google.com/o/oauth2/auth";
    char* token_uri = "https://oauth2.googleapis.com/token";
    char* auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs";
    char* client_secret = "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";
    char* redirect_uri = "https://localhost:8080";
    int state = rand();

    char uri[1024];
    sprintf(uri, 
        "https://accounts.google.com/o/oauth2/auth?"
        "response_type=code&"
        "client_id=%s&"
        "redirect_uri=%s&"
        "scope=https://www.googleapis.com/auth/admin.directory.user.readonly&"
        "state=%d&"
        "access_type=offline",
        client_id, redirect_uri, state
    );

    char auth[1036] = "wslview ";
    strcat(auth, uri);
    system(auth);
}
