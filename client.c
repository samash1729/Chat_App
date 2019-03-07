#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>

#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define BLU "\033[1;34m"

int main(void)
{
    int sockfd,retval,status;
    int rec_bytes,sent_bytes;
    struct sockaddr_in serveraddr;
    char msg[80];
    pid_t p;
    sockfd = socket (AF_INET,SOCK_STREAM,0);
    assert (sockfd != -1 );

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons (3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect (sockfd,(struct sockaddr*) &serveraddr,sizeof
            (serveraddr));
    assert (retval != -1);
    printf(GREEN"Connected"RESET"\n");
    if ( (p = fork()) == 0 )
    {
        while (1)
        {
            rec_bytes = recv (sockfd,msg,sizeof(msg),0);
            assert (rec_bytes != -1);
            if ( strcmp (msg,"bye") == 0)
            {
                printf (GREEN"Received bye message. Disconnecting from server\n"RESET);
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
            waitpid (-1, &status, WNOHANG);
            if ( WIFEXITED(status) )
            {
                kill (p,SIGINT);
                exit(0);
            }
            scanf (" %[^\n]s",msg);
            sent_bytes = send (sockfd,msg,sizeof(msg),0);
            assert (sent_bytes != -1);
            if ( strcmp (msg,"bye") == 0 )
            {
                printf (GREEN"Bye received in parent process client.c"RESET"\n");
                kill (p,SIGINT);
                break;
            }
        }
        exit(0);
    }
    close (sockfd);
    return 0;
}
