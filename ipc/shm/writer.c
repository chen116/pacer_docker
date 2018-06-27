#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#define SHMOBJ_PATH  "/shm_AOS"
#define SEM_PATH     "/sem_AOS"
struct shared_data {
	char var1[10];
	int var2;
};
int main(int argc, char *argv[]) {
	int shared_seg_size = (1 * sizeof(struct shared_data));
	int shmfd  = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWU | S_IRWG);
	ftruncate(shmfd, shared_seg_size);
	struct shared_data * shared_msg = (struct shared_data *)
	mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED,shmfd, 0);
	sem_t * sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
	struct shared_data out_msg = { “John”, 23 };
	sem_wait(sem_id);
	/* Update shared data */
	memcpy(shared_msg, &out_msg, sizeof(struct shared_data));
	sem_post(sem_id);
	shm_unlink(SHMOBJ_PATH);
	sem_close(sem_id);
	sem_unlink(SEM_PATH);
	return 0;
}