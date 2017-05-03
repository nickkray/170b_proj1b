//#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include "synch.h"


#ifndef RWLOCK_H
#define RWLOCK_H
class RWLock{
private:
#ifdef P1_RWLOCK
  //do a bunch of stuff
  int WR, AR; //number of waiting readers and active readers
  int WW, AW; //number of waiting writers and active writers

  Condition* okToWrite; 
  Condition* okToRead;

  Lock* wrt;

#elif P1_SEMAPHORE
  Semaphore* sem;
#elif P1_LOCK
	Lock* wrt;
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


#endif

