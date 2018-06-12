#include <stdio.h>
#include <heartbeats/heartbeat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

void setshm_glo(key_t);
void setshm_log(key_t,int64_t);
void write_vlog(_heartbeat_record_t*,_HB_global_state_t*, int);
int get_hr(_heartbeat_record_t*, int);
int64_t get_ts(_heartbeat_record_t*, int);
int get_index(_HB_global_state_t*);

int get_index(_HB_global_state_t* g)
{
	return g->read_index;
}
int get_hr(_heartbeat_record_t* p,int index)
{
	return p[index].instant_rate*1000000;
}
int64_t get_ts(_heartbeat_record_t* p,int index)
{                                          

 // return p[index].timestamp%10000;
  return p[index].timestamp;
  //return p[index].timestamp;
}
void write_vlog(_heartbeat_record_t* p,_HB_global_state_t* g, int fname_index)
{ 
    FILE * fp;
    char * str1 = "vlog";
    char str2[10];
    sprintf(str2, "%d", fname_index);   
    char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
    strcpy(str3, str1);
    strcat(str3, str2);
    /* open the file for writing*/
    fp = fopen (str3,"w");

 
   /* write 10 lines of text into the file stream*/
   //for(int i = 0; i < g->buffer_index;i++){
   for(int i = 0; i < 51;i++){
    fprintf(fp,"%lld    %d    %lld    %f    %f    %f\n",
          (long long int) p[i].beat,
          p[i].tag,
          (long long int) p[i].timestamp,
          p[i].global_rate,
          p[i].window_rate,
          p[i].instant_rate);
   }
 
   /* close the file*/  
   fclose (fp);
   return;

// printf("%d    %d    %d   %f   %f\n",
//       g->buffer_index,
//       g->counter,
//       g->read_index,
//         g->min_heartrate,
//       g->max_heartrate);

//    return g->counter;
}

void setshm_glo(key_t key)
{

    // key_t key;
    int shmid;

    if ((shmid = shmget(key, 1*sizeof(_HB_global_state_t), IPC_CREAT | 0666)) < 0) {
         perror("shmget");
        // printf("%s",strerror(errno));
        perror("Error in Shared Memory get statement");
        //shmid = shmget(key, SHMSIZE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | IPC_CREAT);
        if (shmid == -1)  {
                // printf("%s",strerror(errno));
                perror("Error in Shared Memory get statement");
                exit(1);
        }
 
    }
    return;

}
void setshm_log(key_t key, int64_t buffer_size )
{

    // key_t key;
    int shmid;

    if ((shmid = shmget(key, buffer_size*sizeof(_heartbeat_record_t), IPC_CREAT | 0666)) < 0) {
         perror("shmget");
        // printf("%s",strerror(errno));
        perror("Error in Shared Memory get statement");
        //shmid = shmget(key, SHMSIZE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | IPC_CREAT);
        if (shmid == -1)  {
                // printf("%s",strerror(errno));
                perror("Error in Shared Memory get statement");
                exit(1);
        }
 
    }
    return;

}