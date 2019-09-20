#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in client_addr, server_addr;
    socklen_t client_addr_size;
    int server_socket;
    int client_socket;

    FILE *fp;

    char filename[BUFFSIZE] = {0};

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVERPORT);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        printf("Can't allocate server_socket\n");
        exit(1);
    }

    if (bind(server_socket, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        printf("Bind Error\n");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        printf("Listen Error\n");
        exit(1);
    }

    while (1) {

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);

        if (client_socket == -1) {
            printf("Connect Error\n");
            close(client_socket);
            exit(1);
        }

        // receive filename
        if (recv(client_socket, filename, BUFFSIZE, 0) == -1) {
            printf("Can't receive filename\n");
        }

        // write file
        if ((fp = fopen(filename, "wb")) == NULL) {
            printf("Can't open file\n");
        }

        receive_file(client_socket, fp);

        // file save or empty file erase
        if (fp != NULL) {
            fseek(fp, 0, SEEK_END);
            if (ftell(fp) != 0) {
                printf("%s Receive Success\n", filename);
                fclose(fp);
            }
            else {
                remove(filename);
            }
        }
        fflush(stdout);
        close(client_socket);
    }
}

void receive_file(int sockfd, FILE *fp) {
    ssize_t n;
    char buff[BUFFSIZE] = {0};
    while ((n = recv(sockfd, buff, BUFFSIZE, 0)) > 0) {
        fwrite(buff, sizeof(char), n, fp);
    }
}