#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
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

    printf("%s\n", uri);

    char view[1038] = "wslview ";
    strcat(view, uri);
    printf("%s\n", view);

    system(view);

    return 0;
}