#include "rwlock.h"

RWLock::RWLock(){
#ifdef RWLOCK
  WR = 0; AR = 0;
  WW = 0; AW = 0;

  pthread_cond_init(&okToRead, NULL);
  pthread_cond_init(&okToWrite, NULL);
  pthread_mutex_init(&lock, NULL);
#else
  pthread_mutex_init(&lock, NULL);
#endif
}

RWLock::~RWLock(){
#ifdef RWLOCK
  pthread_cond_destroy(&okToRead);
  pthread_cond_destroy(&okToWrite);
  pthread_mutex_destroy(&lock);
#else
  pthread_mutex_destroy(&lock);
#endif
}

void RWLock::startRead(){
#ifdef RWLOCK
  pthread_mutex_lock(&lock);
  this->WR++;
  while(AW > 0 || WW > 0){ 
    pthread_cond_wait(&okToRead, &lock); //waiting
  }
  this->WR--;
  this->AR++;
  pthread_mutex_unlock(&lock);
#else
  pthread_mutex_lock(&lock);
#endif
}

void RWLock::doneRead(){
#ifdef RWLOCK
  pthread_mutex_lock(&lock);
  this->AR--;
  if(this->AR == 0 && this->WW > 0)
    pthread_cond_signal(&okToWrite);
  pthread_mutex_unlock(&lock);
#else
  pthread_mutex_unlock(&lock);
#endif
}

void RWLock::startWrite(){
#ifdef RWLOCK
  pthread_mutex_lock(&lock);
  this->WW++;
  while((AW + AR) > 0){
    pthread_cond_wait(&okToWrite, &lock); //waiting
  }
  this->WW--;
  this->AW++;
  pthread_mutex_unlock(&lock);
#else
  pthread_mutex_lock(&lock);
#endif
}

void RWLock::doneWrite(){
#ifdef RWLOCK
  pthread_mutex_lock(&lock);
  AW--;
  if(WW > 0)
    pthread_cond_signal(&okToWrite); //signal one writer
  else if(WR > 0)
    pthread_cond_broadcast(&okToRead); //broadcast to all readers
  pthread_mutex_unlock(&lock);
#else
  pthread_mutex_unlock(&lock);
#endif
}
