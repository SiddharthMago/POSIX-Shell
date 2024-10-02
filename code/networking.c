#include "functions.h"

void iMan_function(char* command) {
    printnl();
    char* token = strtok(command, " ");
    token = strtok(NULL, " ");
    if(token == NULL) {
        printe("ERROR: No token specified\n");
        printnl();
        return;
    }

    char* hostname = "man.he.net";
    struct hostent* server = gethostbyname(hostname);
    if(server == NULL) {
        printe("Error: Unknown server host\n");
        printnl();
        return;
    }

    int socketret = socket(AF_INET, SOCK_STREAM, 0);
    if(socketret == -1) {
        printe("ERROR: Socket failed\n");
        printnl();
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if(connect(socketret, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printe("ERROR: Could not connect to server\n");
        close(socketret);
        printnl();
        return;
    }

    char* request = (char*)malloc(MAX_LENGTH * sizeof(char));
    snprintf(request, MAX_LENGTH, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\nUser-Agent: iMan/1.0\r\nConnection: close\r\n\r\n", token);

    if(send(socketret, request, strlen(request), 0) == -1) {
        printe("ERROR: Could not send request to server\n");
        close(socketret);
        printnl();
        return;
    }

    char* response = (char*)malloc(MAX_LENGTH * sizeof(char));
    int header_skipped = 0;
    int print_content = 0;
    int bytes_received;

    while((bytes_received = recv(socketret, response, MAX_LENGTH - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        
        if(!header_skipped) {
            char* body = strstr(response, "\r\n\r\n");
            if(body) {
                header_skipped = 1;
                strcpy(response, body + 4);
            }
        }

        char* line = strtok(response, "\n");
        while(line != NULL) {
            if(strstr(line, "<PRE>") != NULL) print_content = 1;
            else if(strstr(line, "</PRE>") != NULL) print_content = 0;

            if(print_content == true && strstr(line, "<") == NULL) printf("%s\n\n", line);

            line = strtok(NULL, "\n");
        }
    }

    if(bytes_received == -1) printe("ERROR: Did not recieve response from server\n");

    close(socketret);
    free(request);
    free(response);
    printnl();
    return;
}