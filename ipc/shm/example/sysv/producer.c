#include "shared.h"

void insert_item(int item, int semid, int *shared_buffer) {
  int index = get_buffer_size(shared_buffer);
  shared_buffer[index] = item; 
}

int produce_item() {
  return 0xFF; // nothing dynamic just write a static integer a slot
}

int main(int argc, const char *argv[])
{
  int *shared_buffer = create_shared_mem_buffer();
  int semid = create_semaphore_set();

  clear_buffer(shared_buffer); // prepare buffer for jobs

  int item = 0;
    char temp_buf [10];

    while (temp_buf[0]!='q') {
    fgets (temp_buf, 2, stdin);
    item = (int)temp_buf[0];//produce_item();
    printf("well my int is %d\n",item);
    semop(semid, &downEmpty, 1);
    semop(semid, &downMutex, 1);
    insert_item(item, semid, shared_buffer);
    debug_buffer(shared_buffer);
    semop(semid, &upMutex, 1);
    semop(semid, &upFull, 1);
  }
 
  return EXIT_SUCCESS;
}