#ifndef TRANSFER_FILE_TRANSFER_H
#define TRANSFER_FILE_TRANSFER_H

#define SERVERPORT 8877
#define BUFFSIZE 4096

void receive_file(int sockfd, FILE *fp);

#endif