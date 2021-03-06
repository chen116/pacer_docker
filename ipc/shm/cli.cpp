// w create 2 mem, w read one, r write anthoer
//This is the process main process. Creates the shared memory, 
//places there the integer array and starts integers one by one, blocking if the array is full:

#include <boost/interprocess/sync/interprocess_semaphore.hpp>

struct shared_memory_buffer
{
   enum { NumItems = 5 };

   shared_memory_buffer()
      : mutex(1), nempty(NumItems), nstored(0)
   {}

   //Semaphores to protect and synchronize access
   boost::interprocess::interprocess_semaphore
      mutex, nempty, nstored;

   //Items to fill
   int items[NumItems];
};
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>


using namespace boost::interprocess;

int main ()
{
   //Remove shared memory on construction and destruction
   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("MySharedMemory"); shared_memory_object::remove("vic_MySharedMemory");}
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); shared_memory_object::remove("vic_MySharedMemory");}
   } remover;

   //Create a shared memory object.
   shared_memory_object shm
      (create_only                  //only create
      ,"MySharedMemory"              //name
      ,read_write  //read-write mode
      );

   //Set size
   shm.truncate(sizeof(shared_memory_buffer));

   //Map the whole shared memory in this process
   mapped_region region
      (shm                       //What to map
      ,read_write //Map it as read-write
      );

   //Get the address of the mapped region
   void * addr       = region.get_address();

   //Construct the shared structure in memory
   shared_memory_buffer * data = new (addr) shared_memory_buffer;


   //Create a shared memory object.
   shared_memory_object vic_shm
      (create_only                  //only create
      ,"vic_MySharedMemory"              //name
      ,read_write  //read-write mode
      );

   //Set size
   vic_shm.truncate(sizeof(shared_memory_buffer));

   //Map the whole shared memory in this process
   mapped_region vic_region
      (vic_shm                       //What to map
      ,read_write //Map it as read-write
      );


   const int NumMsg = 10;

   //Insert data in the array
   for(int i = 0; i < NumMsg; ++i){
      data->nempty.wait();
      data->mutex.wait();
      data->items[i % shared_memory_buffer::NumItems] = i;
      printf("wrote: %d\n",i );
      data->mutex.post();
      data->nstored.post();
   }


   void * vic_addr       = vic_region.get_address();

   //Obtain the shared structure
   shared_memory_buffer * vic_data = static_cast<shared_memory_buffer*>(vic_addr);
   int extracted_vic_data [NumMsg];

   //Extract the data
   for(int i = 0; i < NumMsg; ++i){
      vic_data->nstored.wait();
      vic_data->mutex.wait();
      extracted_vic_data[i] = vic_data->items[i % shared_memory_buffer::NumItems];
      printf("got: %d\n",extracted_vic_data[i]);
      vic_data->mutex.post();
      vic_data->nempty.post();
   }

   return 0;
}


// w create sharedMem, open vic_shared. r read sharedMem, create vic_shared
// //This is the process main process. Creates the shared memory, 
// //places there the integer array and starts integers one by one, blocking if the array is full:

// #include <boost/interprocess/sync/interprocess_semaphore.hpp>

// struct shared_memory_buffer
// {
//    enum { NumItems = 5 };

//    shared_memory_buffer()
//       : mutex(1), nempty(NumItems), nstored(0)
//    {}

//    //Semaphores to protect and synchronize access
//    boost::interprocess::interprocess_semaphore
//       mutex, nempty, nstored;

//    //Items to fill
//    int items[NumItems];
// };
// #include <boost/interprocess/shared_memory_object.hpp>
// #include <boost/interprocess/mapped_region.hpp>
// #include <iostream>


// using namespace boost::interprocess;

// int main ()
// {
//    //Remove shared memory on construction and destruction
//    struct shm_remove
//    {
//       shm_remove() { shared_memory_object::remove("MySharedMemory"); shared_memory_object::remove("vic_MySharedMemory");}
//       ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); shared_memory_object::remove("vic_MySharedMemory");}
//    } remover;

//    //Create a shared memory object.
//    shared_memory_object shm
//       (create_only                  //only create
//       ,"MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Set size
//    shm.truncate(sizeof(shared_memory_buffer));

//    //Map the whole shared memory in this process
//    mapped_region region
//       (shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped region
//    void * addr       = region.get_address();

//    //Construct the shared structure in memory
//    shared_memory_buffer * data = new (addr) shared_memory_buffer;


//    const int NumMsg = 10;

//    //Insert data in the array
//    for(int i = 0; i < NumMsg; ++i){
//       data->nempty.wait();
//       data->mutex.wait();
//       data->items[i % shared_memory_buffer::NumItems] = i;
//       printf("wrote: %d\n",i );
//       data->mutex.post();
//       data->nstored.post();
//    }

//    //Create a shared memory object.
//    shared_memory_object vic_shm
//       (open_only                    //only create
//       ,"vic_MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Map the whole shared memory in this process
//    mapped_region vic_region
//       (vic_shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped region
//    void * vic_addr       = vic_region.get_address();

//    //Obtain the shared structure
//    shared_memory_buffer * vic_data = static_cast<shared_memory_buffer*>(vic_addr);


//    int extracted_vic_data [NumMsg];

//    //Extract the data
//    for(int i = 0; i < NumMsg; ++i){
//       vic_data->nstored.wait();
//       vic_data->mutex.wait();
//       extracted_vic_data[i] = vic_data->items[i % shared_memory_buffer::NumItems];
//       printf("got: %d\n",extracted_vic_data[i]);
//       vic_data->mutex.post();
//       vic_data->nempty.post();
//    }

//    return 0;
// }



// //This is the process main process. Creates the shared memory, 
// //places there the integer array and starts integers one by one, blocking if the array is full:

// #include <boost/interprocess/sync/interprocess_semaphore.hpp>

// struct shared_memory_buffer
// {
//    enum { NumItems = 5 };

//    shared_memory_buffer()
//       : mutex(1), nempty(NumItems), nstored(0)
//    {}

//    //Semaphores to protect and synchronize access
//    boost::interprocess::interprocess_semaphore
//       mutex, nempty, nstored;

//    //Items to fill
//    int items[NumItems];
// };
// #include <boost/interprocess/shared_memory_object.hpp>
// #include <boost/interprocess/mapped_region.hpp>
// #include <iostream>


// using namespace boost::interprocess;

// int main ()
// {
//    //Remove shared memory on construction and destruction
//    struct shm_remove
//    {
//       shm_remove() { shared_memory_object::remove("MySharedMemory"); }
//       ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
//    } remover;

//    //Create a shared memory object.
//    shared_memory_object shm
//       (create_only                  //only create
//       ,"MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Set size
//    shm.truncate(sizeof(shared_memory_buffer));

//    //Map the whole shared memory in this process
//    mapped_region region
//       (shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped region
//    void * addr       = region.get_address();

//    //Construct the shared structure in memory
//    shared_memory_buffer * data = new (addr) shared_memory_buffer;

//    const int NumMsg = 10;

//    //Insert data in the array
//    for(int i = 0; i < NumMsg; ++i){
//       data->nempty.wait();
//       data->mutex.wait();
//       data->items[i % shared_memory_buffer::NumItems] = i;
//       printf("wrote: %d\n",i );
//       data->mutex.post();
//       data->nstored.post();
//    }

//    return 0;
// }