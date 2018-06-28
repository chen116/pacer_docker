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
    char temp_buf[10];

    // while (temp_buf[0]!='1') {
    // gets (temp_buf);
    // item = (temp_buf[0]-'0') & 0xff;//produce_item();
    int cnt=10;
    while(cnt>=0){
      cnt++;
    printf("well my int is %d\n",item);
     item = produce_item();
    semop(semid, &downEmpty, 1);
    semop(semid, &downMutex, 1);
    insert_item(item, semid, shared_buffer);
    debug_buffer(shared_buffer);
    semop(semid, &upMutex, 1);
    semop(semid, &upFull, 1);
  }
 
  return EXIT_SUCCESS;
}