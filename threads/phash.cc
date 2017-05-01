#include <iostream>
#include "phash.h"
#include <pthread.h>
#include <unistd.h>

LinkedHashEntry:: LinkedHashEntry(int key, int value) {
  this->key = key;
  this->value = value;
  this->next = NULL;
}

int LinkedHashEntry:: getKey() {
  return key;
}

int LinkedHashEntry:: getValue() {
  return value;
}

void LinkedHashEntry:: setValue(int value) {
  this->value = value;
}


LinkedHashEntry * LinkedHashEntry:: getNext() {
  return next;
}

void LinkedHashEntry:: setNext(LinkedHashEntry *next) {
  this->next = next;
}


const int TABLE_SIZE = 128;

HashMap::HashMap() {
  tableLock = new RWLock();
  table = new LinkedHashEntry*[TABLE_SIZE];
  for (int i = 0; i < TABLE_SIZE; i++)
    table[i] = NULL;
}

int HashMap:: get(int key) {
  //usleep(1);
  tableLock->startRead();
  int hash = (key % TABLE_SIZE);
  if (table[hash] == NULL){
    tableLock->doneRead();
    return -1;
  }
  else {
    LinkedHashEntry *entry = table[hash];
    while (entry != NULL && entry->getKey() != key)
      entry = entry->getNext();
    if (entry == NULL){
      tableLock->doneRead();
      return -1;
    }
    else{
      int retVal = entry->getValue();
      tableLock->doneRead();
      return retVal;
    }
  }
}

void HashMap::put(int key, int value) {
  tableLock->startWrite();
  int hash = (key % TABLE_SIZE);
  if (table[hash] == NULL)
    table[hash] = new LinkedHashEntry(key, value);
  else {
    LinkedHashEntry *entry = table[hash];
    while (entry->getNext() != NULL)
      entry = entry->getNext();
    if (entry->getKey() == key)
      entry->setValue(value);
    else
      entry->setNext(new LinkedHashEntry(key, value));
  }
  tableLock->doneWrite();
}

void HashMap:: remove(int key) {
  tableLock->startWrite();
  int hash = (key % TABLE_SIZE);
  if (table[hash] != NULL) {
    LinkedHashEntry *prevEntry = NULL;
    LinkedHashEntry *entry = table[hash];
    while (entry->getNext() != NULL && entry->getKey() != key) {
      prevEntry = entry;
      entry = entry->getNext();
    }
    if (entry->getKey() == key) {
      if (prevEntry == NULL) {
	LinkedHashEntry *nextEntry = entry->getNext();
	delete entry;
	table[hash] = nextEntry;
      } else {
	LinkedHashEntry *next = entry->getNext();
	delete entry;
	prevEntry->setNext(next);
      }
    }
  }
  tableLock->doneWrite();
}

HashMap:: ~HashMap() {
  for (int i = 0; i < TABLE_SIZE; i++)
    if (table[i] != NULL) {
      LinkedHashEntry *prevEntry = NULL;
      LinkedHashEntry *entry = table[i];
      while (entry != NULL) {
	prevEntry = entry;
	entry = entry->getNext();
	delete prevEntry;
      }
    }
  delete[] table;
  delete this->tableLock;
}


