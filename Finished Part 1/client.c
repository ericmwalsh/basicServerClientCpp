#include <iostream>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <netdb.h> 

using namespace std;


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    while(1){
        int sockfd, portno, n;

        struct sockaddr_in serv_addr;
        struct hostent *server;

        char buffer[5012];
        if (argc < 3) {
           fprintf(stderr,"usage %s hostname port\n", argv[0]);
           exit(0);
        }
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) error("ERROR opening socket");
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");
        printf("File or directory to ls for: ");
        bzero(buffer,5012);
        fgets(buffer,5011,stdin);
        if(strcmp(buffer,"DONE\n")==0){
            n = write(sockfd,"DONE\n",18);
            return 0;
        }
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,5012);
        n = read(sockfd,buffer,5011);
        if (n < 0) error("ERROR reading from socket");
        printf("%s\n",buffer);
    }
    return 0;
}
