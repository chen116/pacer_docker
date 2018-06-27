#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#define SHMOBJ_PATH  "/shm_AOS"
#define SEM_PATH     "/sem_AOS"
struct shared_data {
	char var1[10];
	int var2;
};
int main(int argc, char *argv[]) {
	int shared_seg_size = (1 * sizeof(struct shared_data));
	int shmfd  = shm_open(SHMOBJ_PATH, O_RDONLY, 0666);

	struct shared_data * shared_msg = (struct shared_data *) mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);
	
	
	sem_t * sem_id = sem_open(SEM_PATH, 0);
	
	
	struct shared_data in_msg;
	printf("meow\n");
	
	
	sem_wait(sem_id);
	printf("meow\n");

	/* Update shared data */
	memcpy(&in_msg, shared_msg, sizeof(struct shared_data));
	printf("meow\n");

	sem_post(sem_id);
	printf("meow\n");

	/* Process data… */
	shm_unlink(SHMOBJ_PATH);
	printf("meow\n");

	sem_close(sem_id);
	printf("meow\n");

	sem_unlink(SEM_PATH);
	printf("meow\n");
	
	return 0;
}
