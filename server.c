#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

#define LOOPBACK_ADD "127.0.0.1"

#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define BLU "\033[1;34m"

int main(void)
{
    int sockfd,retval,newsockfd,status = -1;
    int rec_bytes,sentbytes;
    char msg[80];
    struct sockaddr_in serveraddr,clientaddr;
    pid_t p;
    sockfd = socket (AF_INET,SOCK_STREAM,0);
    assert (sockfd != -1);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons (3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // special stuff to make the socket reuse the older address
    int enable =1;
    retval = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    assert (retval >= 0);


    retval = bind (sockfd,(struct sockaddr*) &serveraddr,sizeof (serveraddr));
    assert (retval != -1);

    retval = listen (sockfd,1);
    assert (retval != -1);
    socklen_t actuallen = sizeof(clientaddr);
    newsockfd = accept (sockfd,(struct sockaddr*)&clientaddr,&actuallen);
    assert (newsockfd != -1);

    printf (GREEN"Connected"RESET"\n");
    if ( (p = fork()) == 0 )
    {
        while (1)
        {
            rec_bytes = recv (newsockfd,msg,sizeof(msg),0);
            assert (rec_bytes != -1);
            if ( strcmp (msg,"bye") == 0)
            {
                printf (GREEN"Bye received. Exiting..\n"RESET);
                close (newsockfd);
                close (sockfd);
                kill (getppid(), SIGINT);
                exit(0);
            }
            printf (BLU"%s"RESET"\n",msg);
        }
        exit(0);
    }
    else
    {
        while (1)
        {
            waitpid (-1,&status,WNOHANG);
            if ( WIFEXITED(status) )
            {
                close (newsockfd);
                close (sockfd);
                kill (p,SIGINT);
                exit(0);
            }
            scanf (" %[^\n]s",msg);
            sentbytes = send (newsockfd, &msg,sizeof(msg),0);
            assert (sentbytes != -1);
            if ( strcmp (msg,"bye") == 0 )
            {
                printf (GREEN"bye received"RESET"\n");
                kill (p,SIGINT);
                break;
            }
        }
        exit(0);
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
