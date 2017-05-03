#include "rwlock.h"
#include "synch.h"

RWLock::RWLock(){
#ifdef P1_RWLOCK
  //WR = 0; AR = 0;
  //WW = 0; AW = 0;

 okToRead = new Condition("let's rw bae");
 okToWrite = new Condition("maybe we'll use this too");
  
  wrt = new Lock("lock bae");
#elif P1_SEMAPHORE
   sem = new Semaphore("sem bae", 1);
#elif P1_LOCK
	wrt = new Lock("lock bae");
#else
  pthread_mutex_init(&lock, NULL);
#endif
}

RWLock::~RWLock(){
#ifdef P1_RWLOCK
  okToRead->~Condition();
  okToWrite->~Condition();
wrt->~Lock();
#elif P1_SEMAPHORE
   sem->~Semaphore();
#elif P1_LOCK
	wrt->~Lock();
#else
  pthread_mutex_destroy(lock);
#endif
}

void RWLock::startRead(){
#ifdef P1_RWLOCK
  wrt->Acquire();
  this->WR++;
  while(AW > 0 || WW > 0){ 
    okToRead->Wait(wrt); //waiting
  }
  this->WR--;
  this->AR++;
  wrt->Release();
#elif P1_SEMAPHORE
    sem->P();
#elif P1_LOCK
	wrt->Acquire();
#else
  pthread_mutex_lock(&lock);
#endif
}

void RWLock::doneRead(){
#ifdef P1_RWLOCK
  wrt->Acquire();
  this->AR--;
  if(this->AR == 0 && this->WW > 0)
    okToWrite->Signal(wrt);
  wrt->Release();

#elif P1_SEMAPHORE
    sem->V();
#elif P1_LOCK
	wrt->Release();
#else
  pthread_mutex_unlock(&lock);
#endif
}

void RWLock::startWrite(){
#ifdef P1_RWLOCK
  wrt->Acquire();
  this->WW++;
  while((AW + AR) > 0){
    okToWrite->Wait(wrt); //waiting
  }
  this->WW--;
  this->AW++;
  wrt->Release();
#elif P1_SEMAPHORE
    sem->P();
#elif P1_LOCK
	wrt->Acquire();
#else
  pthread_mutex_lock(&lock);
#endif
}

void RWLock::doneWrite(){
#ifdef P1_RWLOCK
  wrt->Acquire();
  AW--;
  if(WW > 0)
    okToWrite->Signal(wrt); //signal one writer
  else if(WR > 0)
    okToRead->Broadcast(wrt); //broadcast to all readers
  wrt->Release();
#elif P1_SEMAPHORE
    sem->V();
#elif P1_LOCK
	wrt->Release();
#else
  pthread_mutex_unlock(&lock);
#endif
}
