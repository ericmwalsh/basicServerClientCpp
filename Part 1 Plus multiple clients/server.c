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
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");
     
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        int pid = fork();
        if (pid < 0){
            perror("ERROR on fork");
            exit(1);
        }
        if (pid == 0){
            close(sockfd);
            bzero(buffer,256);
            n = read(newsockfd,buffer,255);
            if(strcmp(buffer,"DONE\n")==0){
                n = write(newsockfd,"Connection closed.",18);
                if (n < 0) error("ERROR writing to socket");
                break;
            }
            if (n < 0) error("ERROR reading from socket");
            printf("%s\n",buffer); 
            char buffer2[262]="";
            strcat(buffer2, "ls -l ");
            strcat(buffer2, buffer);
            FILE *in;
            char buff[2048]="";
            if(!(in = popen(buffer2, "r"))) error("ERROR piping ls command");
            char buff3[5012]="";
            while(fgets(buff, 2048, in)!=NULL) strcat(buff3,buff);
            n = write(newsockfd, buff3, 5011);
            if (n < 0) error("ERROR writing to socket");
            pclose(in);
            exit(0);
        }
        else{
            close(newsockfd);
        } 
    }
    return 0; 
 }
