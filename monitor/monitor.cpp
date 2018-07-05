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
#include <heartbeats/heartbeat.h>
#include <sys/shm.h>

#include <vector>


int main (int argc, char **argv)
{
    std::vector <int> clients;
    clients.push_back(1);
    std::vector<int>::iterator it;

    it = find (clients.begin(), clients.end(), 30);
    if (it != clients.end())
        std::cout << "Element found in clients: " << *it << '\n';
    else
        std::cout << "Element not found in clients\n";


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



    // get the oldest message with highest priority
    if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
        perror ("Server: mq_receive");
        exit (1);
    }
    printf ("Server: message received init:%s\n",in_buffer);
    if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
    }

    int i;char bs[1];
    sscanf(in_buffer, "%s%d", bs,&i);
    int shmkey = atoi(in_buffer);
    printf("shmkey %d\n",shmkey);
    double tempRetVal;

    // int shmid_rec;
    // if ((shmid_rec = shmget(shmkey << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
    //     perror("shmget_rec");
    //     return 0;
    // }
    // heartbeat_record_t* hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);


    // int shmid_state;
    // if ((shmid_state = shmget( (shmkey << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
    //     perror("shmget2");
    //     return 0;
    // }
    // HB_global_state_t* hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);
    // heartbeat_record_t* init_hb_rec = hb_rec;
    heartbeat_record_t* hb_rec ;
    heartbeat_record_t* init_hb_rec ;
    HB_global_state_t* hb_state ;



    int cnt = 0;



    int seen = 0 ;

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
        printf ("Server: message received:%s %d\n",in_buffer,token_number);
        if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            continue;
        }

        if (seen==0)
        {
            seen=1;
            int shmid_rec;
            if ((shmid_rec = shmget(shmkey << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
                perror("shmget_rec");
                return 0;
            }
            hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);
            init_hb_rec = hb_rec;

            int shmid_state;
            if ((shmid_state = shmget( (shmkey << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
                perror("shmget2");
                return 0;
            }
            hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);


        }




        hb_rec=    init_hb_rec +      hb_state->counter-1;
        printf("hb rec:%s %f\n",in_buffer,hb_rec->instant_rate );
        printf("hb_state: counter: %s %ld\n", in_buffer, hb_state->counter-1);



        // heartbeat_record_t* meow = hb->log;
        // printf("hb: %f\n",meow->instant_rate );
        cnt++;



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