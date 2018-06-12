#include <stdio.h>
#include <heartbeats/heartbeat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static void hb_flush_buffer(heartbeat_t volatile * );
static float hb_window_average(heartbeat_t volatile * ,int64_t );
_heartbeat_record_t* HB_alloc_log(int , int64_t );
_HB_global_state_t* HB_alloc_state(int );

int anchors_heartbeat_finish(int) ;
int64_t anchors_heartbeat( int, int );
float get_instant_heartrate(int , int);
float get_window_heartrate(int , int);
float get_global_heartrate(int , int);
int anchors_heartbeat_init(int,int64_t,int64_t ,const char* , double ,double );




float get_instant_heartrate(int anchors_hb_shm_key, int index)
{
  int shmid;
  double tempRetVal;
  if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), 0666)) < 0) {
        perror("shmget");
        return 0;
    }
  heartbeat_t* hb = (heartbeat_t*) shmat(shmid, NULL, 0);
  tempRetVal = hb->log[index].instant_rate;
  if(hb!=NULL) {
    shmdt(hb);
  }
  return tempRetVal;
       


}




float get_window_heartrate(int anchors_hb_shm_key, int index)
{
  int shmid;
  double tempRetVal;
  if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), 0666)) < 0) {
        perror("shmget");
        return 0;
    }
  heartbeat_t* hb = (heartbeat_t*) shmat(shmid, NULL, 0);
  tempRetVal = hb->log[index].window_rate;
  if(hb!=NULL) {
    shmdt(hb);
  }
  return tempRetVal;

}
float get_global_heartrate(int anchors_hb_shm_key, int index)
{
  int shmid;
  double tempRetVal;
  if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), 0666)) < 0) {
        perror("shmget");
        return 0;
    }
  heartbeat_t* hb = (heartbeat_t*) shmat(shmid, NULL, 0);
  tempRetVal = hb->log[index].global_rate;
  if(hb!=NULL) {
    shmdt(hb);
  }
  return tempRetVal;

}


int anchors_heartbeat_init(int anchors_hb_shm_key,int64_t window_size,
                            int64_t buffer_depth,
                            const char* log_name,
                            double min_target,
                            double max_target) {
  int pid = getpid();

  char* enabled_dir;
    int shmid;

if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), IPC_CREAT | 0666)) < 0) {
  return 0; 
    }

heartbeat_t* hb = NULL;
hb = (heartbeat_t*) shmat(shmid, NULL, 0); 

  if (hb == NULL) {
    perror("Failed to malloc heartbeat");
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }
  // set to NULL so free doesn't fail in finish function if we have to abort
  hb->window = NULL;
  hb->text_file = NULL;

  
  hb->state = HB_alloc_state(anchors_hb_shm_key);

  if (hb->state == NULL) {

    anchors_heartbeat_finish(anchors_hb_shm_key);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }
  hb->state->pid = pid;

  if(0){//log_name != NULL) {   //vic kill log
    hb->text_file = fopen(log_name, "w");
    if (hb->text_file == NULL) {
      perror("Failed to open heartbeat log file");
      anchors_heartbeat_finish(anchors_hb_shm_key);
      if(hb!=NULL) {
        shmdt(hb);
      }
      return 0;
    } else {
      fprintf(hb->text_file, "Beat    Tag    Timestamp    Global Rate    Window Rate    Instant Rate\n" );
    }
  } else {
    hb->text_file = NULL;
  }

  enabled_dir = getenv("HEARTBEAT_ENABLED_DIR");
  if(!enabled_dir) {
    anchors_heartbeat_finish(anchors_hb_shm_key);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }
  snprintf(hb->filename, sizeof(hb->filename), "%s/%d", enabled_dir, hb->state->pid);


  // printf("%s\n", hb->filename);

  // hb->log = HB_alloc_log(hb->state->pid, buffer_depth);
  

  hb->log = HB_alloc_log(anchors_hb_shm_key, buffer_depth);



  if(hb->log == NULL) {
    anchors_heartbeat_finish(anchors_hb_shm_key);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }

  hb->first_timestamp = hb->last_timestamp = -1;
  hb->state->window_size = window_size;
  hb->window = (int64_t*) malloc((size_t)window_size * sizeof(int64_t));
  if (hb->window == NULL) {
    perror("Failed to malloc window size");
    anchors_heartbeat_finish(anchors_hb_shm_key);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }
  hb->current_index = 0;
  hb->state->min_heartrate = min_target;
  hb->state->max_heartrate = max_target;
  hb->state->counter = 0;
  hb->state->buffer_index = 0;
  hb->state->read_index = 0;
  hb->state->buffer_depth = buffer_depth;
  pthread_mutex_init(&hb->mutex, NULL);
  hb->steady_state = 0;
  hb->state->valid = 0;

  hb->binary_file = fopen(hb->filename, "w");
  if ( hb->binary_file == NULL ) {
    perror("Failed to open heartbeat log");
    anchors_heartbeat_finish(anchors_hb_shm_key);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return 0;
  }
  fclose(hb->binary_file);

  if(hb!=NULL) {
    shmdt(hb);
  }
  return 1;
}
int anchors_heartbeat_finish(int anchors_hb_shm_key) {
    int shmid;
  
  if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), 0666)) < 0) {
        perror("shmget");
        return 0;
    }

  heartbeat_t* hb = (heartbeat_t*) shmat(shmid, NULL, 0);
  if (hb != NULL) {
    pthread_mutex_destroy(&hb->mutex);
    free(hb->window);
    if(0){//hb->text_file != NULL) { //vic kill log
      hb_flush_buffer(hb);
      fclose(hb->text_file);
    }
    remove(hb->filename);
    /*TODO : need to deallocate log */
  }
  if(hb!=NULL) {
    shmdt(hb);
  }
  return 1;
}

int64_t anchors_heartbeat( int anchors_hb_shm_key, int tag )
{
    int shmid;
  
  if ((shmid = shmget(anchors_hb_shm_key, 1*sizeof(heartbeat_t), 0666)) < 0) {
        perror("shmget");
        return 0;
    }
  heartbeat_t* hb = (heartbeat_t*) shmat(shmid, NULL, 0);

    struct timespec time_info;
    int64_t time;
    int64_t old_last_time;

    pthread_mutex_lock(&hb->mutex);
    //printf("Registering Heartbeat\n");
    old_last_time = hb->last_timestamp;
    clock_gettime( CLOCK_REALTIME, &time_info );
    time = ( (int64_t) time_info.tv_sec * 1000000000 + (int64_t) time_info.tv_nsec );
    hb->last_timestamp = time;

    if(hb->first_timestamp == -1) {
      //printf("In heartbeat - first time stamp\n");
      hb->first_timestamp = time;
      hb->last_timestamp  = time;
      hb->window[0] = 0;

      //printf("             - accessing state and log\n");
      hb->log[0].beat = hb->state->counter;
      hb->log[0].tag = tag;
      hb->log[0].timestamp = time;
      hb->log[0].window_rate = 0;
      hb->log[0].instant_rate = 0;
      hb->log[0].global_rate = 0;
      hb->state->counter++;
      hb->state->buffer_index++;
      hb->state->valid = 1;
    }
    else {
      //printf("In heartbeat - NOT first time stamp - read index = %d\n",hb->state->read_index );
      int64_t index =  hb->state->buffer_index;
      hb->last_timestamp = time;
      double window_heartrate = hb_window_average(hb, time-old_last_time);
      double global_heartrate =
  (((double) hb->state->counter+1) /
   ((double) (time - hb->first_timestamp)))*1000000000.0;
      double instant_heartrate = 1.0 /(((double) (time - old_last_time))) *
  1000000000.0;

      hb->log[index].beat = hb->state->counter;
      hb->log[index].tag = tag;
      hb->log[index].timestamp = time;
      hb->log[index].window_rate = window_heartrate;
      hb->log[index].instant_rate = instant_heartrate;
      hb->log[index].global_rate = global_heartrate;
      hb->state->buffer_index++;
      hb->state->counter++;
      hb->state->read_index++;

      if(hb->state->buffer_index%hb->state->buffer_depth == 0) {
  // if(hb->text_file != NULL) //vic kill log
    // hb_flush_buffer(hb);  //vic kill log
  hb->state->buffer_index = 0;
      }
      if(hb->state->read_index%hb->state->buffer_depth == 0) {
  hb->state->read_index = 0;
      }
    }
    pthread_mutex_unlock(&hb->mutex);
    if(hb!=NULL) {
      shmdt(hb);
    }
    return time;

}
static float hb_window_average(heartbeat_t volatile * hb,
              int64_t time) {
  int i;
  double average_time = 0;
  double fps;


  if(!hb->steady_state) {
    hb->window[hb->current_index] = time;

    for(i = 0; i < hb->current_index+1; i++) {
      average_time += (double) hb->window[i];
    }
    average_time = average_time / ((double) hb->current_index+1);
    hb->last_average_time = average_time;
    hb->current_index++;
    if( hb->current_index == hb->state->window_size) {
      hb->current_index = 0;
      hb->steady_state = 1;
    }
  }
  else {
    average_time =
      hb->last_average_time -
      ((double) hb->window[hb->current_index]/ (double) hb->state->window_size);
    average_time += (double) time /  (double) hb->state->window_size;

    hb->last_average_time = average_time;

    hb->window[hb->current_index] = time;
    hb->current_index++;

    if( hb->current_index == hb->state->window_size)
      hb->current_index = 0;
  }
  fps = (1.0 / (float) average_time)*1000000000;

  return (float)fps;
}
static void hb_flush_buffer(heartbeat_t volatile * hb) {
  int64_t i;
  int64_t nrecords = hb->state->buffer_index; // buffer_depth

  //printf("Flushing buffer - %lld records\n",
  //   (long long int) nrecords);

  if(0){//hb->text_file != NULL) { //vic kill log
    for(i = 0; i < nrecords; i++) {
      fprintf(hb->text_file,
        "%lld    %d    %lld    %f    %f    %f\n",
        (long long int) hb->log[i].beat,
        hb->log[i].tag,
        (long long int) hb->log[i].timestamp,
        hb->log[i].global_rate,
        hb->log[i].window_rate,
        hb->log[i].instant_rate);
    }

    fflush(hb->text_file);
  }
}


_heartbeat_record_t* HB_alloc_log(int pid, int64_t buffer_size) {
  _heartbeat_record_t* p = NULL;
  int shmid;

  // printf("Allocating log for %d, %d\n", pid, pid << 1);

  shmid = shmget(pid << 1, buffer_size*sizeof(_heartbeat_record_t), IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("cannot allocate shared memory for heartbeat records");
    return NULL;
  }

  /*
   * Now we attach the segment to our data space.
   */
  p = (_heartbeat_record_t*) shmat(shmid, NULL, 0);
  if (p == (_heartbeat_record_t*) -1) {
    perror("cannot attach shared memory to heartbeat enabled process");
    return NULL;
  }

  return p;
}

_HB_global_state_t* HB_alloc_state(int pid) {
  _HB_global_state_t* p = NULL;
  int shmid;

  shmid = shmget((pid << 1) | 1, 1*sizeof(_HB_global_state_t), IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("cannot allocate shared memory for heartbeat global state");
    return NULL;
  }

  /*
   * Now we attach the segment to our data space.
   */
  p = (_HB_global_state_t*) shmat(shmid, NULL, 0);
  if (p == (_HB_global_state_t*) -1) {
    perror("cannot attach shared memory to heartbeat global state");
    return NULL;
  }

  return p;
}