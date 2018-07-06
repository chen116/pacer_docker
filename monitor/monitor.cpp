#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME_init   "/pacer-srv-mq-gate"
#define SERVER_QUEUE_NAME   "/pacer-srv-mq"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#include <heartbeats/heartbeat.h>
#include <sys/shm.h>

#include <iostream>
#include <boost/thread.hpp>
#include <unordered_map>




// typedef std::unordered_map<int, client> clients_map;
struct client {
  int pid;
  heartbeat_record_t* hb_rec;
  heartbeat_record_t* init_hb_rec;
  HB_global_state_t* hb_state;
  mqd_t qd_client;
} ;
std::unordered_map<int, client> map; 

 
class Monitor{
private:
    // std::unordered_map<int, client> _map;
    boost::mutex * _mutex;
    mqd_t _qd_server;

public:
    // Monitor (std::unordered_map<int, client> map, boost::mutex* mutex) : _map(map), _mutex(mutex)
    Monitor ( boost::mutex* mutex) :  _mutex(mutex)
    {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = MAX_MESSAGES;
        attr.mq_msgsize = MAX_MSG_SIZE;
        attr.mq_curmsgs = 0;

        if ((_qd_server = mq_open ("/pacer-srv-mq", O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
            perror ("Server: mq_open (server)");
            exit (1);
        }        

    }
    ~Monitor()
    {
        mq_close (_qd_server);
        mq_unlink ("/pacer-srv-mq"); 
    }
    void run()
    {
        int token=0;
        while(1)
        {
            token++;
            char in_buffer [MSG_BUFFER_SIZE];
            if (mq_receive (_qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Server: mq_receive");
                exit (1);
            }
            printf ("Server: message received monitor:%s %d\n",in_buffer,token);
            int pid;
            sscanf(in_buffer, "%d",&pid);

            if (pid>0)
            {
                boost::mutex::scoped_lock lock(*_mutex);
                if(map.find (pid)!=map.end())
                {
                    client* cli = &map[pid];
                    cli->hb_rec=     cli->init_hb_rec +      (cli->hb_state->buffer_index-1) ;
                    printf("hb rec:%d %f\n",pid,cli->hb_rec->instant_rate );
                    printf("hb_state: counter: %d %ld\n", pid, cli->hb_state->counter-1);
                    char out_buffer[16];
                    sprintf (out_buffer, "%d", token);
                    if (mq_send (cli->qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
                        perror ("Server: Not able to send message to client");
                        continue;
                    }
                }
                else
                {
                    printf("badddd\n");
                }
            }

        }
    }
}; 
class Gate{
private:
    // std::unordered_map<int, client> _map;
    boost::mutex * _mutex;
    mqd_t _qd_server;

public:
    // Gate (std::unordered_map<int, client> map, boost::mutex* mutex) : _map(map), _mutex(mutex)
    Gate (boost::mutex* mutex) : _mutex(mutex)
    {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = MAX_MESSAGES;
        attr.mq_msgsize = MAX_MSG_SIZE;
        attr.mq_curmsgs = 0;

        if ((_qd_server = mq_open ("/pacer-srv-mq-gate", O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
            perror ("Server: mq_open (server)");
            exit (1);
        }        

    }
    ~Gate()
    {
        mq_close (_qd_server);
        mq_unlink ("/pacer-srv-mq-gate"); 
    }
    void run()
    {
        while(1)
        {
            char in_buffer [MSG_BUFFER_SIZE];
            // get the oldest message with highest priority
            if (mq_receive (_qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Server: mq_receive");
                exit (1);
            }
            printf ("Server: message received init:%s\n",in_buffer);

            int pid;
            sscanf(in_buffer, "%d",&pid);
            printf("gate: pid %d\n",pid);
            if (pid>0)
            {
                boost::mutex::scoped_lock lock(*_mutex);
                if (map.find(pid)==map.end())
                {   
                    printf("new client with pid %d\n",pid);
                    client c;
                    c.pid = pid;
                    char msg[16];
                    sprintf(msg,"/%d",pid);
                    if ((c.qd_client = mq_open (msg, O_WRONLY)) == 1) {
                        perror ("Server: Not able to open client queue");
                    }
                    int shmid_rec;
                    if ((shmid_rec = shmget(pid << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
                        perror("shmget_rec");

                    }
                    c.hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);
                    c.init_hb_rec = c.hb_rec;

                    int shmid_state;
                    if ((shmid_state = shmget( (pid << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
                        perror("shmget2");

                    }
                    c.hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);
                    map[pid]= c;
                    printf("new id: %d\n",map[pid].pid);
                }
                else
                {   
                    printf("closing client with pid %d\n",pid);
                    mq_close(map[pid].qd_client);

                    if (shmdt(map[pid].init_hb_rec)!=0 && shmctl(pid << 1, IPC_RMID, NULL)!=0)
                    {
                        printf("shmdt hb_rec fail\n");
                    }
                    if (shmdt(map[pid].hb_state)!=0 && shmctl((pid << 1) | 1, IPC_RMID, NULL)!=0)
                    {
                        printf("shmdt hb_state fail\n");
                    }


                    map.erase(pid);
                }
            }





        }

    }
};

int main()
{

    boost::mutex mutex;
    // typedef std::unordered_map<int, client> clients_map;
    Gate gate( &mutex);
    Monitor monitor( &mutex);
    
    boost::thread th1(&Gate::run, &gate); 
    boost::thread th2(&Monitor::run, &monitor); 
    th1.join();
    th2.join();    
    return 0;
}


/*
https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux
 * server.c: Server program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>

// #include <fcntl.h>
// #include <sys/stat.h>
// #include <mqueue.h>

// #define SERVER_QUEUE_NAME_init   "/pacer-srv-mq-gate"
// #define SERVER_QUEUE_NAME   "/pacer-srv-mq"
// #define QUEUE_PERMISSIONS 0660
// #define MAX_MESSAGES 10
// #define MAX_MSG_SIZE 256
// #define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
// #include <heartbeats/heartbeat.h>
// #include <sys/shm.h>



// #include <pthread.h>


// mqd_t qd_server,qd_server_init;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t q_not_empty;


// int main (int argc, char **argv)
// {
//     pthread_t qd_server_init_tid,qd_server_tid;




//     mqd_t qd_server, qd_client;   // queue descriptors
//     int token_number = 1; // next token to be given to client

//     printf ("Servering: Hello, World!\n");

//     struct mq_attr attr;
//     attr.mq_flags = 0;
//     attr.mq_maxmsg = MAX_MESSAGES;
//     attr.mq_msgsize = MAX_MSG_SIZE;
//     attr.mq_curmsgs = 0;

//     if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
//         perror ("Server: mq_open (server)");
//         exit (1);
//     }
//     char in_buffer [MSG_BUFFER_SIZE];
//     char out_buffer [MSG_BUFFER_SIZE];




//     // get the oldest message with highest priority
//     if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
//         perror ("Server: mq_receive");
//         exit (1);
//     }
//     printf ("Server: message received init:%s\n",in_buffer);
//     if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
//         perror ("Server: Not able to open client queue");
//     }


//     // char msg[10];
//     // int pid;
//     // sscanf(in_buffer, "%d %s",&pid,msg);
//     // printf("init:get:  pid: %d msg: %s\n",pid, msg );
//     // int shmkey=pid;


//     int i;char bs[1];
//     sscanf(in_buffer, "%s%d", bs,&i);
//     int shmkey = atoi(in_buffer);
//     printf("shmkey %d\n",shmkey);
//     double tempRetVal;

//     int shmid_rec;
//     if ((shmid_rec = shmget(shmkey << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
//         perror("shmget_rec");
//         return 0;
//     }
//     heartbeat_record_t* hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);


//     int shmid_state;
//     if ((shmid_state = shmget( (shmkey << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
//         perror("shmget2");
//         return 0;
//     }
//     HB_global_state_t* hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);
//     heartbeat_record_t* init_hb_rec = hb_rec;
//     // heartbeat_record_t* hb_rec ;
//     // heartbeat_record_t* init_hb_rec ;
//     // HB_global_state_t* hb_state ;



//     int cnt = 0;



//     int seen = 0 ;
//     printf("entering while loop:\n");
//     while (1) {
//         // mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL);
//         // qd_client = mq_open (in_buffer, O_WRONLY);
//         // mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0);
//         // mq_close(qd_client);
        

    


//         // get the oldest message with highest priority
//         if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
//             perror ("Server: mq_receive");
//             exit (1);
//         }
//         printf ("Server: message received:%s %d\n",in_buffer,token_number);
//         if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
//             perror ("Server: Not able to open client queue");
//             continue;
//         }




//         hb_rec=    init_hb_rec +      (hb_state->buffer_index-1) ;
//         printf("hb rec:%s %f\n",in_buffer,hb_rec->instant_rate );
//         printf("hb_state: counter: %s %ld\n", in_buffer, hb_state->counter-1);

//         cnt++;



//         sprintf (out_buffer, "%d", token_number);
//         if (mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
//             perror ("Server: Not able to send message to client");
//             continue;
//         }





//         mq_close(qd_client);
//         printf ("Server: response sent to client. %d\n",token_number);
//         token_number++;
//     }
// }

// /*
// https://www.softprayog.in/programming/interprocess-communication-using-posix-message-queues-in-linux
//  * server.c: Server program
//  *           to demonstrate interprocess commnuication
//  *           with POSIX message queues
//  */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/types.h>

// #include <fcntl.h>
// #include <sys/stat.h>
// #include <mqueue.h>

// #define SERVER_QUEUE_NAME   "/pacer-srv-mq"
// #define QUEUE_PERMISSIONS 0660
// #define MAX_MESSAGES 10
// #define MAX_MSG_SIZE 256
// #define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
// #include <heartbeats/heartbeat.h>
// #include <sys/shm.h>

// #include <vector>


// int main (int argc, char **argv)
// {
//     std::vector <int> clients;
//     clients.push_back(1);
//     std::vector<int>::iterator it;

//     it = find (clients.begin(), clients.end(), 30);
//     if (it != clients.end())
//         std::cout << "Element found in clients: " << *it << '\n';
//     else
//         std::cout << "Element not found in clients\n";




//     mqd_t qd_server, qd_client;   // queue descriptors
//     int token_number = 1; // next token to be given to client

//     printf ("Servering: Hello, World!\n");

//     struct mq_attr attr;

//     attr.mq_flags = 0;
//     attr.mq_maxmsg = MAX_MESSAGES;
//     attr.mq_msgsize = MAX_MSG_SIZE;
//     attr.mq_curmsgs = 0;

//     if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
//         perror ("Server: mq_open (server)");
//         exit (1);
//     }
//     char in_buffer [MSG_BUFFER_SIZE];
//     char out_buffer [MSG_BUFFER_SIZE];



//     // get the oldest message with highest priority
//     if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
//         perror ("Server: mq_receive");
//         exit (1);
//     }
//     printf ("Server: message received init:%s\n",in_buffer);
//     if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
//         perror ("Server: Not able to open client queue");
//     }

//     int i;char bs[1];
//     sscanf(in_buffer, "%s%d", bs,&i);
//     int shmkey = atoi(in_buffer);
//     printf("shmkey %d\n",shmkey);
//     double tempRetVal;

//     // int shmid_rec;
//     // if ((shmid_rec = shmget(shmkey << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
//     //     perror("shmget_rec");
//     //     return 0;
//     // }
//     // heartbeat_record_t* hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);


//     // int shmid_state;
//     // if ((shmid_state = shmget( (shmkey << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
//     //     perror("shmget2");
//     //     return 0;
//     // }
//     // HB_global_state_t* hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);
//     // heartbeat_record_t* init_hb_rec = hb_rec;
//     heartbeat_record_t* hb_rec ;
//     heartbeat_record_t* init_hb_rec ;
//     HB_global_state_t* hb_state ;



//     int cnt = 0;



//     int seen = 0 ;

//     while (1) {
//         // mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL);
//         // qd_client = mq_open (in_buffer, O_WRONLY);
//         // mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0);
//         // mq_close(qd_client);
        

    


//         // get the oldest message with highest priority
//         if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
//             perror ("Server: mq_receive");
//             exit (1);
//         }
//         printf ("Server: message received:%s %d\n",in_buffer,token_number);
//         if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
//             perror ("Server: Not able to open client queue");
//             continue;
//         }

//         if (seen==0)
//         {
//             seen=1;
//             int shmid_rec;
//             if ((shmid_rec = shmget(shmkey << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
//                 perror("shmget_rec");
//                 return 0;
//             }
//             hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);
//             init_hb_rec = hb_rec;

//             int shmid_state;
//             if ((shmid_state = shmget( (shmkey << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
//                 perror("shmget2");
//                 return 0;
//             }
//             hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);


//         }




//         hb_rec=    init_hb_rec +      hb_state->counter-1;
//         printf("hb rec:%s %f\n",in_buffer,hb_rec->instant_rate );
//         printf("hb_state: counter: %s %ld\n", in_buffer, hb_state->counter-1);



//         // heartbeat_record_t* meow = hb->log;
//         // printf("hb: %f\n",meow->instant_rate );
//         cnt++;



//         sprintf (out_buffer, "%d", token_number);
//         if (mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
//             perror ("Server: Not able to send message to client");
//             continue;
//         }





//         mq_close(qd_client);
//         printf ("Server: response sent to client. %d\n",token_number);
//         token_number++;
//     }
// }