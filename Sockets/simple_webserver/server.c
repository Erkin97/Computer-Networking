#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#define PORT 5000

void respond (int sock);

int main( int argc, char *argv[] ) {
  int sockfd, newsockfd, portno = PORT;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  clilen = sizeof(cli_addr);

  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  // port reusable
  int tr = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  /* Initialize socket structure */
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* TODO : Now bind the host address using bind() call.*/
  if ( bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 ){
    perror("bind error");
    exit(1);
  }

  /* TODO : listen on socket you created */

  if ( listen(sockfd, 10) == -1 ){
    perror("listen error");
    exit(1);
  }

  printf("Server is running on port %d\n", portno);

  while (1) {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if ( newsockfd == -1 ){
      perror("accept error");
      exit(1);
    }
    respond(newsockfd);
  }

  return 0;
}

void respond(int sock) {
  int offset, bytes;
  char buffer[9000];
  bzero(buffer,9000);

  offset = 0;
  bytes = 0;
  do {
    // bytes < 0 : unexpected error
    // bytes == 0 : client closed connection
    bytes = recv(sock, buffer + offset, 1500, 0);
    offset += bytes;
    // this is end of http request
    if (strncmp(buffer + offset - 4, "\r\n\r\n", 4) == 0) break;
  } while (bytes > 0);

  if (bytes < 0) {
    printf("recv() error\n");
    return;
  } else if (bytes == 0) {
    printf("Client disconnected unexpectedly\n");
    return;
  }

  buffer[offset] = 0;
  printf("%s\n", buffer);

  char message[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html;\r\n\r\n<html><body>Hello World!</body></html>\r\n\r\n";

  int length = strlen(message);
  while(length > 0) {
    printf("send bytes : %d\n", bytes);
    bytes = send(sock, message, length, 0);
    length = length - bytes;
  }
  printf("close\n");
  shutdown(sock, SHUT_RDWR);
  close(sock);
}
