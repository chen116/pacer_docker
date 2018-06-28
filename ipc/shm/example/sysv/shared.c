#include "shared.h"

/**
 * returns current size of shared buffer
 */
int get_buffer_size(int *sbuff) {
  int i = 0;
  int counter = 0;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    if (sbuff[i] == 0xFF) {
      counter++;
    } 
  }
  return counter;
}

void debug_buffer(int *sbuff) {
  int i = 0;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    if (sbuff[i] == 0xFF) printf("1\n");
  }
  printf("debug meow\n");
}

/**
 * returns a pointer to a shared memory buffer that the
 * producer can write to.
 */
int *create_shared_mem_buffer() {
  int *shmaddr = 0; /* buffer address */
  key_t key = SHM_KEY; /* use key to access a shared memory segment */
  
  int shmid = shmget(key, BUFFER_SIZE, IPC_CREAT | SHM_R | SHM_W); /* give create, read and write access */
  if (errno > 0) {
    perror("failed to create shared memory segment");
    exit (EXIT_FAILURE);
  }

  shmaddr = (int*)shmat(shmid, NULL, 0);
  if (errno > 0) {
    perror ("failed to attach to shared memory segment");
    exit (EXIT_FAILURE);
  }

  // clean out garbage memory in shared memory
  return shmaddr;
}

/**
 * only used in the producer to clean out garbage memory when
 * constructing initial buffer.
 */
void clear_buffer(int *sbuff) {
  int i = 0;
  for (i = 0; i < BUFFER_SIZE; ++i) sbuff[i] = 0x00;
}

/**
 * create FULL and EMPTY semaphores
 */
int create_semaphore_set() {
  key_t key = ftok(SEM_KEY, 'E');
  
  int semid = semget(key, NSEM_SIZE, 0600 | IPC_CREAT);
  if (errno > 0) {
    perror("failed to create semaphore array");
    exit (EXIT_FAILURE);
  } 

  semctl(semid, FULL_ID, SETVAL, 0);
  if (errno > 0) {
    perror("failed to set FULL semaphore");
    exit (EXIT_FAILURE);
  }

  semctl(semid, EMPTY_ID, SETVAL, BUFFER_SIZE);
  if (errno > 0) {
    perror("failed to set EMPTY sempahore");
    exit (EXIT_FAILURE);
  }

  semctl(semid, MUTEX_ID, SETVAL, 1);
  if (errno > 0) {
    perror("failed to create mutex");
  }

  return semid;
}
