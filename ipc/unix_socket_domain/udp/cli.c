#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>


#define DATA "The sea is calm tonight, the tide is full . . ."


/*
 * Send a datagram to a receiver whose name is specified in the command
 * line arguments.  The form of the command line is <programname> <pathname>
 */


int main()
    // int argc;
    // char *argv[];
{
    int sock;
    struct sockaddr_un name;


    /* Create socket on which to send. */
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("opening datagram socket");
        exit(1);
    }
    /* Construct name of socket to send to. */
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, "socket_vic");
    /* Send message. */
    if (sendto(sock, DATA, sizeof(DATA), 0,
        &name, sizeof(struct sockaddr_un)) < 0) {
        perror("sending datagram message");
    }


    char buf[1024];
    /* Read from the socket */
    rc = recv(sock, buf, sizeof(buf),MSG_PEEK);



    close(sock);
}