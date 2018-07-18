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
#include <map>



// typedef std::unordered_map<int, client> clients_map;
struct client {
  int cnt;
  int pid;
  heartbeat_record_t* hb_rec;
  heartbeat_record_t* init_hb_rec;
  HB_global_state_t* hb_state;
  mqd_t qd_client;
heartbeat_t* heart;
double priority=0;
double last_ts=0;
double last_hr=0;


} ;
std::unordered_map<int, client> clients_map;  //<pid, client>





 
class Monitor{
private:
    // std::unordered_map<int, client> _map;
    boost::mutex * _mutex;
    mqd_t _qd_server;
    std::multimap<int,client*> clients_task_queue; //<< pid , client>
    int busy=0;



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

        while(1)
        {
            
            char in_buffer [MSG_BUFFER_SIZE];
            if (mq_receive (_qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Server: mq_receive");
                exit (1);
            }
            // printf ("Server: message received monitor:%s\n",in_buffer);
            int pid;
            int finished;
            sscanf(in_buffer, "%d %d",&pid,&finished);

            printf ("Server: message received monitor: pid:%d, finished:%d\n",pid,finished);
            if (pid>0)
            {
                boost::mutex::scoped_lock lock(*_mutex);
                if(clients_map.find (pid)!=clients_map.end())
                {

                    client* cli = &clients_map[pid];
                    if(!finished)
                        heartbeat(cli->heart,1);
                    printf("instant_rate from heart %f\n", hb_get_instant_rate(cli->heart));
                    cli->hb_rec=     cli->init_hb_rec +      (cli->hb_state->buffer_index-1) ;
                    printf("hb rec instant rate:%d %f\n",pid,cli->hb_rec->instant_rate );
                    printf("hb_state: counter: %d %ld\n", pid, cli->hb_state->counter-1);
                    printf("tag: counter: %d %d\n", pid, cli->hb_rec->tag);

                    double cur_ts = hbr_get_timestamp(cli->hb_rec)/1000000000;
                    printf("cur_ts %f\n", cur_ts);

                    //update_priority()
                    for (auto update_it = clients_map.begin(); update_it != clients_map.end(); ++update_it) 
                    {

                        printf(" updated ittt:: pid :%d ,$dcli->pri %f, cli->last_ts %f, cli->last_hr %f",update_it->first ,update_it->second.priority, update_it->second.last_ts,update_it->second.last_hr );

                        update_it->second.priority = update_it->second.last_hr / (1+ cur_ts - update_it->second.last_ts);
                        if (update_it->second.priority < 0 )
                        {
                            printf(" neggggggg %f %f \n",cur_ts ,update_it->second.last_ts  );
                        }

                        printf(" cli-> new priority %f \n",update_it->second.priority);
                    }
                    cli->priority = hb_get_instant_rate(cli->heart);
                    cli->last_ts = cur_ts;
                    cli->last_hr = hb_get_instant_rate(cli->heart);
                    printf(" updated: cli->pri %f, cli->last_ts %f, cli->last_hr %f\n",cli->priority,cli->last_ts,cli->last_hr );



                    if (!busy)
                    {
                        if (!finished)
                        {
                            char out_buffer[16];
                            sprintf (out_buffer, "%d", cli->cnt);
                            cli->cnt++;
                            if (mq_send (cli->qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
                                perror ("Server: Not able to send message to client");
                                continue;
                            }
                            busy=pid;
                        }
                    }
                    else
                    {
                        if(finished)
                        {
                            if (clients_task_queue.size()==0)
                            {
                                busy=0;
                                // char out_buffer[16];
                                // sprintf (out_buffer, "%d", cli->cnt);
                                // if (mq_send (cli->qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
                                //     perror ("Server: Not able to send message to client");
                                //     continue;
                                // }
                            }
                            else
                            {

                                // who got to run based on who has smallest priorty:
                                int pid_get_to_run = 0;
                                double smallest_pri = 9999999;
                                client* popped_cli ;
                                // std::multimap<double,client*>::iterator it_get_to_run;



                                for ( auto itt = clients_task_queue.begin(); itt != clients_task_queue.end(); ++itt )
                                {
                                    printf("priority when gpu busy: %d\n",itt->second->pid );
                                    if (itt->second->priority<smallest_pri)
                                    {
                                        pid_get_to_run = itt->second->pid;
                                        smallest_pri = itt->second->priority;
                                        popped_cli =  (itt->second);
                                        // it_get_to_run = itt;
                                    }
                                }
                                printf("popped_cli: %d , pri: %f\n", popped_cli->pid,popped_cli->priority);







                                // std::multimap<double,client*>::iterator it = clients_task_queue.begin();
                                // client* popped_cli  = (*it).second;
                                char out_buffer[16];
                                sprintf (out_buffer, "%d", popped_cli->cnt);
                                popped_cli->cnt++;
                                if (mq_send (popped_cli->qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
                                    perror ("Server: Not able to send message to client");
                                    continue;
                                }
                                busy=popped_cli->pid;
                                clients_task_queue.erase(pid_get_to_run);
                                printf("next running task: %d\n",busy );
                                printf("meow map conatins:\n");
                                for ( auto itt = clients_map.begin(); itt != clients_map.end(); ++itt )
                                    printf("      pid  %d, pri: %f\n",itt->first , itt->second.priority );
                            }
                        }
                        else
                        {

                            //insert the cli that requested gpu

                            clients_task_queue.insert (std::pair<int,client*>(cli->pid,cli) );
                            printf("GPU busy:\n");
                            for (std::multimap<int,client*>::iterator it = clients_task_queue.begin();it != clients_task_queue.end();++it)
                            {
                                printf("pid: %d , pri:%f\n", (*it).first ,(*it).second->priority);
                            }

                        }
                    }







                    // client* cli = &clients_map[pid];
                    // cli->hb_rec=     cli->init_hb_rec +      (cli->hb_state->buffer_index-1) ;
                    // printf("hb rec instant rate:%d %f\n",pid,cli->hb_rec->instant_rate );
                    // printf("hb_state: counter: %d %ld\n", pid, cli->hb_state->counter-1);
                    // char out_buffer[16];
                    // sprintf (out_buffer, "%d", cli->cnt);
                    // cli->cnt++;
                    // if (mq_send (cli->qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
                    //     perror ("Server: Not able to send message to client");
                    //     continue;
                    // }





                }
                else
                {
                    printf("pid not found in map\n");
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
            printf ("Server gate thread: message received:%s\n",in_buffer);

            int pid;
            sscanf(in_buffer, "%d",&pid);
            printf("gate: pid %d\n",pid);
            if (pid>0)
            {
                boost::mutex::scoped_lock lock(*_mutex);
                if (clients_map.find(pid)==clients_map.end()) // first time, create client
                {   
                    printf("new client with pid %d\n",pid);
                    client c;
                    char msg[16];
                    sprintf(msg,"/%d",pid);
                    if ((c.qd_client = mq_open (msg, O_WRONLY)) == 1) {
                        perror ("Server: Not able to open client queue");
                    }

                    char hb_log_file[16];
                    sprintf(hb_log_file,"log_%d",pid);
                    c.heart = heartbeat_init(10, 100, hb_log_file, 100, 100 );
                    printf("hb inited at monitor!\n");
                    int shmid_rec=-1;
                    int shmid_state=-1;                    
                    while (shmid_rec< 0)
                    {
                        shmid_rec = shmget(pid << 1, 100*sizeof(heartbeat_record_t), 0666);
                    }
                    while (shmid_state<0)
                    {
                        shmid_state = shmget( (pid << 1) | 1, sizeof(HB_global_state_t), 0666);
                    }



                    // int shmid_rec;
                    // if ((shmid_rec = shmget(pid << 1, 100*sizeof(heartbeat_record_t), 0666)) < 0) {
                    //     perror("shmget_rec");

                    // }
                    // int shmid_state;
                    // if ((shmid_state = shmget( (pid << 1) | 1, sizeof(HB_global_state_t), 0666)) < 0) {
                    //     perror("shmget2");

                    // }


                    c.hb_rec = (heartbeat_record_t*) shmat(shmid_rec, NULL, 0);
                    c.init_hb_rec = c.hb_rec;
                    c.hb_state = (HB_global_state_t*) shmat(shmid_state, NULL, 0);
                    c.cnt=0;
                    c.pid=pid;
                    clients_map[pid]= c;

                    printf("new id inited!: %d\n",pid);
                }
                else // second time, closing client
                {   
                    printf("closing client shm with pid %d\n",pid);
                    mq_close(clients_map[pid].qd_client);

                    heartbeat_finish(clients_map[pid].heart);

                    if (shmdt(clients_map[pid].init_hb_rec)==0 && shmctl(shmget(pid << 1, 100*sizeof(heartbeat_record_t), 0666), IPC_RMID, NULL)==0 )
                    {
                        perror("shmdt/ctl hb_rec ");

                    }
                    if (shmdt(clients_map[pid].hb_state)==0 && shmctl(shmget( (pid << 1) | 1, sizeof(HB_global_state_t), 0666), IPC_RMID, NULL)==0 )
                    {
                        perror("shmdt/ctl hb_state ");
                    }
                    clients_map.erase(pid);
                }
                printf("map conatins:\n");
                for ( auto it = clients_map.begin(); it != clients_map.end(); ++it )
                    printf("        %d\n",it->first );
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


