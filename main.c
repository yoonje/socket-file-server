#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in clientaddr, serveraddr;
    socklen_t addrlen = sizeof(clientaddr);
    int server_socket;
    int client_socket;

    char filename[BUFFSIZE] = {0};
    FILE *fp = fopen(filename, "wb");
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        perror("Can't allocate server_socket");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    if (bind(server_socket, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("Bind Error");
        exit(1);
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("Listen Error");
        exit(1);
    }

    client_socket = accept(server_socket, (struct sockaddr *) &clientaddr, &addrlen);
    if (client_socket == -1)
    {
        perror("Connect Error");
        exit(1);
    }
    close(server_socket);

    if (recv(client_socket, filename, BUFFSIZE, 0) == -1)
    {
        perror("Can't receive filename");
        exit(1);
    }

    if (fp == NULL)
    {
        perror("Can't open file");
        exit(1);
    }

    receive_file(client_socket, fp);

    fclose(fp);
    close(client_socket);
    return 0;
}

void receive_file(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0)
    {
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
}