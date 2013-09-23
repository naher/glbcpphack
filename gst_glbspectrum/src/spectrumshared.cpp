#include "spectrumshared.h"
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/optional.hpp>


#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#define MUTEXNAME "myMutex"
#define MEMALLOC 4194304

	  // mutex to access the message vector (myVector)
      struct mutex_access
      {
	  public:
		  static boost::interprocess::named_mutex & getMutex() {
			  return getInstance().mutex;
		  }

	  protected:
		  static mutex_access & getInstance() {
			  static mutex_access instance;
			  return instance;
		  }

	  protected:
         mutex_access() 
			 : mutex(boost::interprocess::open_or_create, MUTEXNAME)
		  { boost::interprocess::named_mutex::remove(MUTEXNAME); }
         ~mutex_access() { boost::interprocess::named_mutex::remove(MUTEXNAME); }

		 boost::interprocess::named_mutex mutex;
      };

	  // condition to know if ogre was initialized
	  typedef struct init_flag {
		  //Mutex to protect access to the condition
		  boost::interprocess::interprocess_mutex mutex;

		  //Condition to wait when ogre is not initialized
		  boost::interprocess::interprocess_condition  cond_started;

		  bool initialized;

		  init_flag() : initialized(false) {}
	  } InitFlag;
	  
typedef boost::interprocess::allocator<Message, boost::interprocess::managed_shared_memory::segment_manager>  ShmemAllocator;

typedef boost::interprocess::vector<Message, ShmemAllocator> MyVector;

MyVector * myVector;

InitFlag * initFlag;

int lastMessage;

int * writtenMsgs, * readMsgs, * lastRead;


#ifdef __cplusplus
extern "C" {
#endif

	struct shm_remove
    {
        shm_remove() { boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
        ~shm_remove(){ boost::interprocess::shared_memory_object::remove("MySharedMemory"); }
    };

void createSharedMem() {
	static shm_remove remover;

	//Create a new segment with given name and size
	static boost::interprocess::managed_shared_memory segment(boost::interprocess::create_only, "MySharedMemory", MEMALLOC);

	//Initialize shared memory STL-compatible allocator
	const ShmemAllocator alloc_inst (segment.get_segment_manager());

	//Construct a vector named "MyVector" in shared memory with argument alloc_inst
	myVector = segment.construct<MyVector>("MyVector")(alloc_inst);

	//Construct a structure to control init process
	initFlag = segment.construct<InitFlag>("InitFlag")();

	//Construct an int to know the last written message
	writtenMsgs = segment.construct<int>("WrittenMessage")();

	//Construct an int to know the last read message
	readMsgs = segment.construct<int>("ReadMessage")();

	//Construct an int to know the last read message
	lastRead = segment.construct<int>("LastRead")();

	*writtenMsgs = 0;
	*readMsgs = 0;
	*lastRead = 0;
}

/*
* Checks to see if Ogre was initialized
* [IMPORTANT] pre: initFlag != NULL
*/
void checkInit() {
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(initFlag->mutex);
	while (!initFlag->initialized) {
		initFlag->cond_started.wait(lock);
	}
}

/*
* Set the cond variable to reflect Ogre's init status
*/
void setInit(bool value) {
	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(initFlag->mutex);
	initFlag->initialized = value;
	initFlag->cond_started.notify_all();
}

void useSharedMem() {

	//Create a new segment with given name and size
	static boost::interprocess::managed_shared_memory segment(boost::interprocess::open_only, "MySharedMemory");

	//Initialize shared memory STL-compatible allocator
	const ShmemAllocator alloc_inst (segment.get_segment_manager());

	//Construct a vector named "MyVector" in shared memory with argument alloc_inst
	myVector = segment.find<MyVector>("MyVector").first;

	if (myVector)
		lastMessage = 0;

	initFlag = segment.find<InitFlag>("InitFlag").first;

	//Construct an int to know the last written message
	writtenMsgs = segment.find<int>("WrittenMessage").first;

	//Construct an int to know the last read message
	readMsgs = segment.find<int>("ReadMessage").first;

	//Construct an int to know the last read message
	lastRead = segment.find<int>("LastRead").first;

}

void writeMessage(Message * m) {

	boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex_access::getMutex());

	if (!myVector)
		return;

	for (int i = 0; i < m->spectrum_bands; i++) {
		if (m->magnitude[i] > -60)
			std::cout << m->magnitude[i];
	}
	
	// deallocates unused space
	if (MEMALLOC/2 < sizeof(Message) * (myVector->size())) {
		myVector->erase(myVector->begin(), myVector->begin()+20);
		(*lastRead) -= 20;
		if (*lastRead < 0) *lastRead = 0;
	}

	myVector->push_back(*m);
	(*writtenMsgs)++;
	//printf("writeMessage: Pushed back. (%p), size: %d \n", myVector, myVector->size());
		
}

void readMessage(Message * m) {

	boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex_access::getMutex());

	if (!myVector)
		return;

	//if (lastMessage < myVector->size()) { // something available to read
	if ((*readMsgs < *writtenMsgs) && (*lastRead < (int)myVector->size())) {
		Message const& aux = myVector->at((*lastRead)++);
		(*readMsgs)++;
		//for (int i = 0; i < aux.spectrum_bands; i++)
		//	m->magnitude[i] = aux.magnitude[i];

		std::copy(aux.magnitude, aux.magnitude + aux.spectrum_bands, m->magnitude);

		for (int i = 0; i < m->spectrum_bands; i++) {
			if (m->magnitude[i] > -60)
				std::cout << m->magnitude[i];
		}

		m->spectrum_bands = aux.spectrum_bands;
		m->timestamp = aux.timestamp;
		//printf("readMessage #%d: Popped back. \n", lastMessage-1);
	} else {
		//for (int i = 0; i < 128; i++)
		//	m->magnitude[i] = 0;

		std::fill_n(m->magnitude, 128, 0); 
		printf("readMessage: No new messages. \n");
	}

}

#ifdef __cplusplus
}
#endif