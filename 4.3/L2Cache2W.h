#ifndef SIMPLECACHE_H
#define SIMPLECACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

/************************ Utils ************************/
unsigned int getBlockOffset(uint32_t);
unsigned int getLineIndex(uint32_t);
unsigned int getTag(uint32_t);

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);
void initDRAM();

/*********************** Cache Line *************************/
typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  uint8_t Data[BLOCK_SIZE];
  uint32_t Time; /* Used for LRU */
} CacheLine;

/*********************** L1Cache *************************/

/* 
L1_LINES = L1_SIZE / BLOCK_SIZE = 256 

L1_LINES = 256
*/
#define L1_LINES 256

typedef struct L1Cache {
  CacheLine line[L1_LINES];
} L1Cache;

void initL1();

void accessL1(uint32_t, uint8_t *, uint32_t);

/*********************** L2Cache *************************/

/* 
L2_SETS = L2_SIZE / BLOCK_SIZE / WAYS = 256

L2_SETS = 256
*/

#define L2_SETS 256
#define WAYS 2


typedef struct Sets {
  CacheLine line[WAYS];
} Sets;

typedef struct L2Cache {
  Sets sets[L2_SETS];
} L2Cache;

void initL2();

void accessL2(uint32_t, uint8_t *, uint32_t);


/*********************** Cache *************************/

typedef struct Cache {
  L1Cache l1;
  L2Cache l2;
} Cache;

void initCache();

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
