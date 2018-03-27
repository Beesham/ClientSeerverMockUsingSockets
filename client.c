#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 40041 // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(int argc, char *argv[])
{
    int serverfd, numbytes;  
    char buf[MAXDATASIZE];
    char s[INET_ADDRSTRLEN];
    socklen_t sin_size;    
    struct sockaddr_in servAdd;

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error: client");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = inet_addr(argv[1]);
    servAdd.sin_port = PORT;

    if ((connect(serverfd, (struct sockaddr*)&servAdd, sizeof(servAdd)) == -1)) {
        close(serverfd);
        perror("client: failed to connect");
        exit(1);
    }


    char buffer[1000];
    char prompt[]=">>";
    size_t nBytes;

    while(1){
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)]='\0';

        int bytesSent = send(serverfd, buffer, strlen(buffer),0);
        printf("Bytes sent: %d\n", bytesSent);
        if(bytesSent < 0) {
            perror("send");
            exit(1);
        }

       // while(1){
            if ((numbytes = recv(serverfd, buf, sizeof(buf), 0)) == -1) {
                perror("recv");
                break;
            }    
            
            //buf[numbytes] = '\0';
            printf("client: received '%s'\n", buf);
            
       // }

        fflush(stdout);
    }//end of while(1)    
    close(serverfd);

    return 0;
}
