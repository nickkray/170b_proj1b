#include "rwlock.h"
#include "synch.h"

RWLock::RWLock(){
#ifdef P1_RWLOCK
  WR = 0; AR = 0;
  WW = 0; AW = 0;

  pthread_cond_init(&okToRead, NULL);
  pthread_cond_init(&okToWrite, NULL);
  pthread_mutex_init(&lock, NULL);
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
  pthread_cond_destroy(&okToRead);
  pthread_cond_destroy(&okToWrite);
  pthread_mutex_destroy(&lock);
#elif P1_SEMAPHORE
   sem->~Semaphore();
#elif P1_LOCK
	wrt->~Lock();
#else
  pthread_mutex_destroy(&lock);
#endif
}

void RWLock::startRead(){
#ifdef P1_RWLOCK
  pthread_mutex_lock(&lock);
  this->WR++;
  while(AW > 0 || WW > 0){ 
    pthread_cond_wait(&okToRead, &lock); //waiting
  }
  this->WR--;
  this->AR++;
  pthread_mutex_unlock(&lock);
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
  pthread_mutex_lock(&lock);
  this->AR--;
  if(this->AR == 0 && this->WW > 0)
    pthread_cond_signal(&okToWrite);
  pthread_mutex_unlock(&lock);

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
  pthread_mutex_lock(&lock);
  this->WW++;
  while((AW + AR) > 0){
    pthread_cond_wait(&okToWrite, &lock); //waiting
  }
  this->WW--;
  this->AW++;
  pthread_mutex_unlock(&lock);
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
  pthread_mutex_lock(&lock);
  AW--;
  if(WW > 0)
    pthread_cond_signal(&okToWrite); //signal one writer
  else if(WR > 0)
    pthread_cond_broadcast(&okToRead); //broadcast to all readers
  pthread_mutex_unlock(&lock);
#elif P1_SEMAPHORE
    sem->V();
#elif P1_LOCK
	wrt->Release();
#else
  pthread_mutex_unlock(&lock);
#endif
}
