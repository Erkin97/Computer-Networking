#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

   char buffer[256];
   portno = 5000;

   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   server = gethostbyname("127.0.0.1");

   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);

   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

   /* Now ask for a message from the user, this message
      * will be read by server
   */
   while (1) {
     printf("What do you want to say? ");
     bzero(buffer,256);
     scanf("%s", buffer);

     /* Send message to the server */
     n = write(sockfd, buffer, strlen(buffer));

     if (n < 0) {
       perror("ERROR writing to socket");
       exit(1);
     }

     /* Now read server response */
     bzero(buffer,256);
     n = read(sockfd, buffer, 255);

     if (n < 0) {
       perror("ERROR reading from socket");
       exit(1);
     }
     printf("server replied : %s\n",buffer);
     if (!bcmp(buffer, "quit", 4)) break;
   }

   return 0;
}
