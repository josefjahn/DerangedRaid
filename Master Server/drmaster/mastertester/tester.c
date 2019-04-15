#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MYPORT 1370    /* the port users will be connecting to */
char ReceiveBuffer[65535];
char SendBuffer[65535];
int PacketSize=10000;
struct sockaddr_in from;
int addr_len, numbytes;


int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in their_addr; /* connector's address information */
    struct hostent *he;
    int numbytes;

    addr_len = sizeof(struct sockaddr);

    if (argc <= 2) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;         /* host byte order */
    their_addr.sin_port = htons(MYPORT);     /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);        /* zero the rest of the struct */


    sprintf(SendBuffer, "%s", "Z 192.168.0.200 Werewolf_Test 8 2 1\0");
    sendto(sockfd,SendBuffer,strlen(SendBuffer),0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
    sprintf(SendBuffer, "%s", "Z 192.168.100.202 Server2 8 2 1\0");
    sendto(sockfd,SendBuffer,strlen(SendBuffer),0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
    sprintf(SendBuffer, "%s", "Z 193.170.255.64 TotalWAR_at_spaceballs.dhs.org 8 0 1\0");
    sendto(sockfd,SendBuffer,strlen(SendBuffer),0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));

    sendto(sockfd,argv[2],strlen(argv[2]),0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));

    printf("sent data to %s\n",inet_ntoa(their_addr.sin_addr));

    do
    {
	numbytes = recvfrom(sockfd,ReceiveBuffer,PacketSize,0,(struct sockaddr *)&from, &addr_len);
	ReceiveBuffer[numbytes] = '\0';
        printf("Received reply: \"%s\"\n",ReceiveBuffer);
    } while (strcmp(ReceiveBuffer, "X Done!") != 0);
    
    close(sockfd);

    return 0;
}



