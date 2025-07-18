#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>         // <-- This defines struct addrinfo
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include "linkedlist/linkedlist.h"

//http://localhost:8080/OAuth2

char* client_id = "55593084363-i1897onpemdnsk3q79m14t1f402e3vri.apps.googleusercontent.com";
char* project_id = "oauth-study-465305";
char* auth_uri = "https://accounts.google.com/o/oauth2/auth";
char* token_uri = "https://oauth2.googleapis.com/token";
char* auth_provider_x509_cert_url = "https://www.googleapis.com/oauth2/v1/certs";
char* client_secret = "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";
char* redirect_uri = "http://localhost:8080";
int state = -1;

void OAuth(int state);
struct linkedlist CodeStateTokenRequest(char* info);
void HandleAccessToken(int client_sock, char* info_start, int state);

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

        int buffer_len = 5012;
        char buffer[buffer_len];
        int bytes_scanned = recv(sock, buffer, buffer_len, 0);
        if(bytes_scanned <= 0) break;
        buffer[bytes_scanned] = '\0';

        printf("request:\n%s\n", buffer);

        char* token = strtok(buffer, " ");
        token = strtok(NULL, " ");
        
        if(state == -1) state = rand();

        if(strcmp(token, "/OAuth2") == 0) {
            OAuth(state);
            continue;
        } 
        
        char* info_start = strchr(token, '?');
        printf("POST area\n");
        if(info_start != NULL) {
            *info_start = 0;
            printf("buffer before POST: %s\n", token);
            if(strcmp(token, "/") == 0) { // the redirect_uri
                printf("access token POST request\n");
                HandleAccessToken(sock, info_start, state);
                state = -1;
            }   
        }
    }
}

void HandleAccessToken(int client_sock, char* info_start, int state) {
    info_start = info_start + 1;
    struct linkedlist fields = CodeStateTokenRequest(info_start);
    char* code = fields.search(&fields, "code");
    char* field_state = fields.search(&fields, "state");
    if(atoi(field_state) != state) {
        printf("state mismatch\n");
        exit(1);
    }

    char* res = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<h1>Authorised!</h1>";
    write(client_sock, res, strlen(res));

    printf("response Authorised sent\n");

    /* the post request body */
    char post_body[1024];
    sprintf(post_body, 
        "grant_type=authorization_code&"
        "client_id=%s&"
        "client_secret=%s&"
        "redirect_uri=%s&"
        "code=%s",
        client_id, client_secret, redirect_uri, code
    );

    /* the post request header */
    char header[1024];
    sprintf(header,
        "POST /token HTTP/1.1\r\n"
        "Host: oauth2.googleapis.com\r\n"
        "Content-length: %ld\r\n"
        "Content-type: application/x-www-form-urlencoded\r\n"
        "\r\n",
        strlen(post_body)
    );

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *rp;
    int sfd;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;

    int s = getaddrinfo("oauth2.googleapis.com", "https", &hints, &result);
    if (s != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    /* getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        If socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address. 
    */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    freeaddrinfo(result);           /* No longer needed */

    if (rp == NULL) {               /* If no address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(1);
    }

    /* initialize OpenSSL - do this once and stash ssl_ctx in a global var */
    SSL_load_error_strings ();
    SSL_library_init ();
    SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());

    if (ssl_ctx == NULL) {
        fprintf(stderr, "ERROR: could not initialize the SSL context: %s\n", strerror(errno));
        exit(1);
    }

    /* create an SSL connection and attach it to the socket */
    SSL *conn = SSL_new(ssl_ctx);
    SSL_set_fd(conn, sfd);

    /* perform the SSL/TLS handshake with the server */
    if (SSL_connect(conn) < 0) {
        int err = SSL_get_error(conn, -1);
        printf("SSL_connect failed with error code %d\n", err);
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* Send header */
    SSL_write(conn, header, strlen(header));

    /* Send body */
    SSL_write(conn, post_body, strlen(post_body));

    /* receive response */
    int response_length = 2049;
    char response[response_length];

    ssize_t n = SSL_read(conn, response, response_length);
    response[2048] = '\0';

    printf("%s\n", response);

    SSL_set_shutdown(conn, SSL_RECEIVED_SHUTDOWN | SSL_SENT_SHUTDOWN);
    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);   
    close(sfd);
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
        close(listen_sock);
        exit(1);
    }

    if(listen(listen_sock, 5) < 0) {
        printf("listen failed\n");
        close(listen_sock);
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

struct linkedlist CodeStateTokenRequest(char* info) {
    struct linkedlist list = linkedListConstructor();

    char* field_start = info;
    int loop = 1;
    while(loop) {
        char* andpersan = strchr(field_start, '&');

        if(andpersan == NULL) loop = 0;
        else *andpersan = 0;
        
        char* field = strtok(field_start, "=");
        char* data = strtok(NULL, "=");
        list.insert(&list, field, data);
        field_start = andpersan + 1;
    }
    return list;
}

void OAuth(int state) {
    char uri[1024];
    sprintf(uri, 
        "https://accounts.google.com/o/oauth2/auth?"
        "response_type=code&"
        "client_id=%s&"
        "redirect_uri=%s&"
        "scope=https://www.googleapis.com/auth/admin.directory.user.readonly&"
        "state=%d&"
        "prompt=consent&"
        "access_type=offline",
        client_id, redirect_uri, state
    );

    char auth[1036];
    sprintf(auth, "wslview \"%s\"", uri);
    system(auth);
}
