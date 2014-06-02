/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <iostream>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     while(1){
        listen(sockfd,5);
         clilen = sizeof(cli_addr);
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);
         if (newsockfd < 0) 
              error("ERROR on accept");
         bzero(buffer,256);
         n = read(newsockfd,buffer,255);
         if(strcmp(buffer,"DONE\n")==0){
            n = write(newsockfd,"Connection closed.",18);
            if (n < 0) error("ERROR writing to socket");
            break;
        }
         if (n < 0) error("ERROR reading from socket");
         printf("%s\n",buffer); 
         char buffer2[259]="";
         strcat(buffer2, "ls ");
         strcat(buffer2, buffer);
         //buffer = ((std::string)"ls " + (std::string)buffer).c_str();
         int stdoutCopy = dup(1);
         int stderrCopy = dup(2);
         dup2(newsockfd, 1);
         dup2(newsockfd, 2);
         system(buffer2);
         dup2(stdoutCopy,1);
         dup2(stderrCopy,2);
         //FILE* file = popen(buffer2, "r");
         n = write(newsockfd,"file recieved",18);
         if (n < 0) error("ERROR writing to socket");
     }
     return 0; 
}
