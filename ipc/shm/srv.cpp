// w create 2 mem, w read one, r write anthoer
//The second process opens the shared memory and copies the received integers to it's own buffer:

#include <boost/interprocess/sync/interprocess_semaphore.hpp>

struct shared_memory_buffer
{
   enum { NumItems = 1 };

   shared_memory_buffer()
      : mutex(1), nempty(1), nstored(0)
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
   //Remove shared memory on destruction
   struct shm_remove
   {
      ~shm_remove(){ shared_memory_object::remove("MySharedMemory");shared_memory_object::remove("docker_MySharedMemory"); }
   } remover;





   //Create a shared memory object.
   shared_memory_object shm
      (create_only                  //only create
      ,"MySharedMemory"              //name
      ,read_write  //read-write mode
      );

   //Map the whole shared memory in this process
   mapped_region region
      (shm                       //What to map
      ,read_write //Map it as read-write
      );

   //Get the address of the mapped region
   void * addr       = region.get_address();

   //Obtain the shared structure
   shared_memory_buffer * data = static_cast<shared_memory_buffer*>(addr);

   const int NumMsg = 1;

   int extracted_data [NumMsg];
   int i = 0
   //Extract the data
   data->nstored.wait();
   data->mutex.wait();
   extracted_data[0] = data->items[0];
   printf("srv got: %d\n",extracted_data[0]);
   data->mutex.post();
   data->nempty.post();


   //Create a shared memory object.
   shared_memory_object docker_shm
      (open_only                  //only create
      ,"docker_MySharedMemory"              //name
      ,read_write  //read-write mode
      );
   //Map the whole shared memory in this process
   mapped_region docker_region
      (docker_shm                       //What to map
      ,read_write //Map it as read-write
      );

   //Get the address of the mapped docker_region
   void * docker_addr       = docker_region.get_address();

   //Construct the shared structure in memory
   shared_memory_buffer * docker_data = new (docker_addr) shared_memory_buffer;
   docker_data->nempty.wait();
   docker_data->mutex.wait();
   docker_data->items[0] = extracted_data[0];
   printf("srv wrote: %d\n",docker_data->items[0] );
   docker_data->mutex.post();
   docker_data->nstored.post();



   return 0;
}

// w create sharedMem, open docker_shared. r read sharedMem, create docker_shared

// //The second process opens the shared memory and copies the received integers to it's own buffer:

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
//    //Remove shared memory on destruction
//    struct shm_remove
//    {
//       ~shm_remove(){ shared_memory_object::remove("MySharedMemory");shared_memory_object::remove("docker_MySharedMemory"); }
//    } remover;

//    //Create a shared memory object.
//    shared_memory_object docker_shm
//       (create_only                  //only create
//       ,"docker_MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Set size
//    docker_shm.truncate(sizeof(shared_memory_buffer));

//    //Map the whole shared memory in this process
//    mapped_region docker_region
//       (docker_shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped docker_region
//    void * docker_addr       = docker_region.get_address();

//    //Construct the shared structure in memory
//    shared_memory_buffer * docker_data = new (docker_addr) shared_memory_buffer;





//    //Create a shared memory object.
//    shared_memory_object shm
//       (open_only                    //only create
//       ,"MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Map the whole shared memory in this process
//    mapped_region region
//       (shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped region
//    void * addr       = region.get_address();

//    //Obtain the shared structure
//    shared_memory_buffer * data = static_cast<shared_memory_buffer*>(addr);

//    const int NumMsg = 10;

//    int extracted_data [NumMsg];

//    //Extract the data
//    for(int i = 0; i < NumMsg; ++i){
//       data->nstored.wait();
//       data->mutex.wait();
//       extracted_data[i] = data->items[i % shared_memory_buffer::NumItems];
//       printf("got: %d\n",extracted_data[i]);
//       data->mutex.post();
//       data->nempty.post();
//    }

//    //Insert data in the array
//    for(int i = 0; i < NumMsg; ++i){
//       docker_data->nempty.wait();
//       docker_data->mutex.wait();
//       docker_data->items[i % shared_memory_buffer::NumItems] = i+10;
//       printf("wrote: %d\n",i );
//       docker_data->mutex.post();
//       docker_data->nstored.post();
//    }


//    return 0;
// }
// //The second process opens the shared memory and copies the received integers to it's own buffer:

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
//    //Remove shared memory on destruction
//    struct shm_remove
//    {
//       ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
//    } remover;

//    //Create a shared memory object.
//    shared_memory_object shm
//       (open_only                    //only create
//       ,"MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Map the whole shared memory in this process
//    mapped_region region
//       (shm                       //What to map
//       ,read_write //Map it as read-write
//       );

//    //Get the address of the mapped region
//    void * addr       = region.get_address();

//    //Obtain the shared structure
//    shared_memory_buffer * data = static_cast<shared_memory_buffer*>(addr);

//    const int NumMsg = 10;

//    int extracted_data [NumMsg];

//    //Extract the data
//    for(int i = 0; i < NumMsg; ++i){
//       data->nstored.wait();
//       data->mutex.wait();
//       extracted_data[i] = data->items[i % shared_memory_buffer::NumItems];
//       printf("got: %d\n",extracted_data[i]);
//       data->mutex.post();
//       data->nempty.post();
//    }
//    return 0;
// }