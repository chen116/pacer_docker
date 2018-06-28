#include "shared.h"

void consume_item(int item) {
  // do something with item
}

int remove_item(int semid, int *shared_buffer) {
  int index = get_buffer_size(shared_buffer) - 1;
  int item = shared_buffer[index];
  shared_buffer[index] = 0x00;
  return item;
}

int main(int argc, const char *argv[])
{
  int *shared_buffer = create_shared_mem_buffer();
  int semid = create_semaphore_set();

  int item = 0;

  while(1) {
    semop(semid, &downFull, 1);
    semop(semid, &downMutex, 1);
    item = remove_item(semid, shared_buffer);
    printf("meow eat\n");
    debug_buffer(shared_buffer);
    semop(semid, &upMutex, 1);
    semop(semid, &upEmpty, 1);
    consume_item(item);
  } 

  return EXIT_SUCCESS;
}