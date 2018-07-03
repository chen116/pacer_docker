// w create 2 mem, w read one, r write anthoer
//The second process opens the shared memory and copies the received integers to it's own buffer:
#include <iostream>
using namespace std;
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
   const int NumMsg = 1;

   struct shm_remove
   {
      shm_remove() { shared_memory_object::remove("pacer-srv-shm"); }
      ~shm_remove(){ shared_memory_object::remove("pacer-srv-shm"); }
   } remover;

   //Create a shared memory object.
   shared_memory_object shm
      (create_only                  //only create
      ,"pacer-srv-shm"              //name
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



   int extracted_data [NumMsg];
   int i=0;

// while(1)
// {
//Extract the data
   // for(int i = 0; i < NumMsg; ++i){
      data->nstored.wait();
      data->mutex.wait();
      extracted_data[i] = data->items[i % shared_memory_buffer::NumItems];
      printf("got: %d\n",extracted_data[i]);
      data->mutex.post();
      data->nempty.post();
   // }
   char cli_shm_name [64];  

   sprintf (cli_shm_name, "/pacer-cli-shm-%d", extracted_data[i]);
   printf("cli shm name: %s\n",cli_shm_name );
   //Create a shared memory object.
   shared_memory_object cli_shm
      (read_only                  //only create
      ,cli_shm_name              //name
      ,read_write  //read-write mode
      );

   //Map the whole shared memory in this process
   mapped_region cli_region
      (cli_shm                       //What to map
      ,read_write //Map it as read-write
      );
   void * cli_addr       = cli_region.get_address();

   //Obtain the shared structure
   shared_memory_buffer * cli_data = static_cast<shared_memory_buffer*>(cli_addr);


   //Insert data in the array
   // for(int i = 0; i < NumMsg; ++i){
      cli_data->nempty.wait();
      cli_data->mutex.wait();
      cli_data->items[i % shared_memory_buffer::NumItems] = i+10;
      printf("wrote: %d\n",i+10 );
      cli_data->mutex.post();
      cli_data->nstored.post();
   // }

// }



   return 0;
}
// // w create 2 mem, w read one, r write anthoer
// //The second process opens the shared memory and copies the received integers to it's own buffer:
// #include <iostream>
// using namespace std;
// #include <boost/interprocess/sync/interprocess_semaphore.hpp>

// struct shared_memory_buffer
// {
//    enum { NumItems = 1 };

//    shared_memory_buffer()
//       : mutex(1), nempty(1), nstored(0)
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
//    const int NumMsg = 1;

//    struct shm_remove
//    {
//       shm_remove() { shared_memory_object::remove("pacer-srv-shm"); shared_memory_object::remove("vic_MySharedMemory");}
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


//    //Create a shared memory object.
//    shared_memory_object vic_shm
//       (create_only                  //only create
//       ,"vic_MySharedMemory"              //name
//       ,read_write  //read-write mode
//       );

//    //Set size
//    vic_shm.truncate(sizeof(shared_memory_buffer));

//    //Map the whole shared memory in this process
//    mapped_region vic_region
//       (vic_shm                       //What to map
//       ,read_write //Map it as read-write
//       );
//    void * vic_addr       = vic_region.get_address();

//    //Obtain the shared structure
//    shared_memory_buffer * vic_data = static_cast<shared_memory_buffer*>(vic_addr);


//    int extracted_data [NumMsg];
//    int i=0;

// while(1)
// {
// //Extract the data
//    // for(int i = 0; i < NumMsg; ++i){
//       data->nstored.wait();
//       data->mutex.wait();
//       extracted_data[i] = data->items[i % shared_memory_buffer::NumItems];
//       printf("got: %d\n",extracted_data[i]);
//       data->mutex.post();
//       data->nempty.post();
//    // }

//    //Insert data in the array
//    // for(int i = 0; i < NumMsg; ++i){
//       vic_data->nempty.wait();
//       vic_data->mutex.wait();
//       vic_data->items[i % shared_memory_buffer::NumItems] = i+10;
//       printf("wrote: %d\n",i+10 );
//       vic_data->mutex.post();
//       vic_data->nstored.post();
//    // }

// }



//    return 0;
// }
