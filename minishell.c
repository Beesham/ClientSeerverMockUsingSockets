/*
    Beesham Sarendranauth
    104854956
    2018/03/29
    Description: creates a client side connection to server. mimics ssh without the security
*/


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

#define MAXDATASIZE 1000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(int argc, char *argv[])
{
    int serverfd, numbytes;  
    char buf[MAXDATASIZE];
    struct sockaddr_in servAdd;

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    //Created the socket
    if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error: client");
        exit(1);
    }

    //Define the connection
    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = inet_addr(argv[1]);
    servAdd.sin_port = PORT;

    //connect to the socket
    if ((connect(serverfd, (struct sockaddr*)&servAdd, sizeof(servAdd)) == -1)) {
        close(serverfd);
        perror("client: failed to connect");
        exit(1);
    }

    char buffer[1000];
    char prompt[]=">>";

    //Read/write to the socket
    while(1){
        //clear the buffers because of repeated use
        memset(&buf, 0, sizeof(buf));
        memset(&buffer, 0, sizeof(buffer));
        
        //get imput from user
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)-1]='\0';

        //send command to the server
        int bytesSent = send(serverfd, buffer, strlen(buffer),0);
        if(bytesSent <= 0) {
           perror("send");
           break; 
        }

        //read responce
        if ((numbytes = recv(serverfd, buf, MAXDATASIZE, 0)) <= 0) {
            printf("Server disconnected\n");
            break;
        }    
        
        printf("%s\n", buf);
        fflush(stdout);
        fflush(stdin);
    }//end of while(1)    
    close(serverfd);
}
