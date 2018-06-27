#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>


// #define BUF_SIZE 1024
// struct shmbuf { // Buffer in shared memory
//  sem_t wsem; // Writer semaphore
//  sem_t rsem; // Reader semaphore
//  int cnt; // Number of bytes used in 'buf'
//  char buf[BUF_SIZE]; // Data being transferred
// }


// int man(){
// 	fd = shm_open(SHM_PATH, O_CREAT|O_EXCL|O_RDWR, OBJ_PERMS);
// 	ftruncate(fd, sizeof(struct shmbuf));
// 	shmp = mmap(NULL, sizeof(struct shmbuf),
// 	 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
// 	sem_init(&shmp->rsem, 1, 0);
// 	sem_init(&shmp->wsem, 1, 1); // Writer gets first turn
// 	for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
// 	 sem_wait(&shmp->wsem); // Wait for our turn
// 	 shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
// 	 sem_post(&shmp->rsem); // Give reader a turn
// 	 if (shmp->cnt == 0) // EOF on stdin?
// 	 break;
// 	}
// 	sem_wait(&shmp->wsem); // Wait for reader to finish


// }
#define SHMOBJ_PATH  "/shm_AOS"
#define SEM_PATH     "/sem_AOS"
struct shared_data {
	char var1[10];
	int var2;
};
int main(int argc, char *argv[]) {
	int shared_seg_size = (1 * sizeof(struct shared_data));
	int shmfd  = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	ftruncate(shmfd, shared_seg_size);
	struct shared_data * shared_msg = (struct shared_data *)
	mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED,shmfd, 0);
	sem_t * sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
	struct shared_data out_msg = { "John", 23 };
	sem_wait(sem_id);
	/* Update shared data */
	memcpy(shared_msg, &out_msg, sizeof(struct shared_data));
	sem_post(sem_id);
	shm_unlink(SHMOBJ_PATH);
	sem_close(sem_id);
	sem_unlink(SEM_PATH);
	return 0;
}
