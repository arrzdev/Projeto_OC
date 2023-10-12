#ifndef L1CACHE_H
#define L1CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

void resetTime();

uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL2(uint32_t, uint8_t *, uint32_t);
void accessL1(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine
{
    uint8_t Valid;
    uint8_t Dirty;
    uint32_t Tag;
    uint8_t Data[BLOCK_SIZE];
} CacheLine;

typedef struct L1
{
    uint32_t init;
    CacheLine lines[L1_SIZE / BLOCK_SIZE];
} L1;

typedef struct L2
{
    uint32_t init;
    CacheLine lines[L2_SIZE / BLOCK_SIZE];
} L2;

typedef struct Cache
{
    uint32_t init;
    L1 l1;
    L2 l2;
} Cache;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
