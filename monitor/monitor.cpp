/*
https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME   "/pacer-srv-mq"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main (int argc, char **argv)
{
    mqd_t qd_server, qd_client;   // queue descriptors
    int token_number = 1; // next token to be given to client

    printf ("Servering: Hello, World!\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    char in_buffer [MSG_BUFFER_SIZE];
    char out_buffer [MSG_BUFFER_SIZE];

    while (1) {
        // mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL);
        // qd_client = mq_open (in_buffer, O_WRONLY);
        // mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0);
        // mq_close(qd_client);
        

    
        // get the oldest message with highest priority
        if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
        printf ("Server: message received:%s\n",in_buffer);
        // send reply message to client
        if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            continue;
        }
        int i;
        char bs[1];
		sscanf(in_buffer, "%s%d", bs,&i);
		int shmkey = atoi(in_buffer);
		printf("shmkey %d\n",shmkey);

        sprintf (out_buffer, "%d", token_number);
        if (mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Server: Not able to send message to client");
            continue;
        }
        mq_close(qd_client);
        printf ("Server: response sent to client. %d\n",token_number);
        token_number++;
    }
}