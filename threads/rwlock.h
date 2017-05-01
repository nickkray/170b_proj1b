//#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

#define RW_LOCK_H

class RWLock{
private:
#ifdef P1_RWLOCK
  //do a bunch of stuff
  int WR, AR; //number of waiting readers and active readers
  int WW, AW; //number of waiting writers and active writers

  pthread_cond_t okToWrite = PTHREAD_COND_INITIALIZER;
  pthread_cond_t okToRead = PTHREAD_COND_INITIALIZER;

  pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

#elif P1_SEMAPHORE
  Semaphore* sem;
#else 
  pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER; 
#endif
  
 public:
#ifdef P1_SEMAPHORE
 
#else
  RWLock();
  ~RWLock();

  //Reader
  void startRead();
  void doneRead();
  // Writer
  void startWrite();
  void  doneWrite();
#endif
};

