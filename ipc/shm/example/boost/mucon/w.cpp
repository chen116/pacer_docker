//the process main process. Creates the shared memory, places there the buffer and starts writing messages one by one until it writes "last message" to indicate that there are no more messages to print:
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

struct trace_queue
{
   enum { LineSize = 100 };

   trace_queue()
      :  message_in(false)
   {}

   //Mutex to protect access to the queue
   boost::interprocess::interprocess_mutex      mutex;

   //Condition to wait when the queue is empty
   boost::interprocess::interprocess_condition  cond_empty;

   //Condition to wait when the queue is full
   boost::interprocess::interprocess_condition  cond_full;

   //Items to fill
   char   items[LineSize];

   //Is there any message
   bool message_in;
};

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>
#include <cstdio>


using namespace boost::interprocess;

int main ()
{

   //Erase previous shared memory and schedule erasure on exit
   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("MySharedMemory"); }
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
   } remover;

   //Create a shared memory object.
   shared_memory_object shm
      (create_only               //only create
      ,"MySharedMemory"           //name
      ,read_write                //read-write mode
      );
   try{
      //Set size
      shm.truncate(sizeof(trace_queue));

      //Map the whole shared memory in this process
      mapped_region region
         (shm                       //What to map
         ,read_write //Map it as read-write
         );

      //Get the address of the mapped region
      void * addr       = region.get_address();

      //Construct the shared structure in memory
      trace_queue * data = new (addr) trace_queue;

      const int NumMsg = 100;

      for(int i = 0; i < NumMsg; ++i){
         scoped_lock<interprocess_mutex> lock(data->mutex);
         if(data->message_in){
            data->cond_full.wait(lock);
         }
         if(i == (NumMsg-1))
            std::sprintf(data->items, "%s", "last message");
         else
            std::sprintf(data->items, "%s_%d", "my_trace", i);

         //Notify to the other process that there is a message
         data->cond_empty.notify_one();

         //Mark message buffer as full
         data->message_in = true;
      }
   }
   catch(interprocess_exception &ex){
      std::cout << ex.what() << std::endl;
      return 1;
   }

   return 0;
}