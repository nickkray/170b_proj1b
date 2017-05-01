//#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

#define RW_LOCK_H

class RWLock{
private:
#ifdef RWLOCK
  //do a bunch of stuff
  int WR, AR; //number of waiting readers and active readers
  int WW, AW; //number of waiting writers and active writers

  pthread_cond_t okToWrite = PTHREAD_COND_INITIALIZER;
  pthread_cond_t okToRead = PTHREAD_COND_INITIALIZER;

  pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
#else 
  pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER; 
#endif
  
 public:
  RWLock();
  ~RWLock();
  //Reader
  void startRead();
  void doneRead();
  // Writer
  void startWrite();
  void  doneWrite();
};
