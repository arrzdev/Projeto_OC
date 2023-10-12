#include "L1Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache SimpleCache;

/**************** Utils ***************/

/*
returns block offset, 6 least significant bits of address

block size = 16 * word size = 16 * 4 = 64 bytes = 2^6 bytes
so, block offset = 6 bits
*/
uint32_t getBlockOffset(uint32_t address) {
  /* 0x3F = 0011 1111 */
  return address & 0x3F;
}

/*
returns line index, 8 bits of address

line size = 256 = 2^8
so, line index = 8 bits
*/
uint32_t getLineIndex(uint32_t address) { 
  /* 0xFF = 1111 111 */
  return (address >> 6) & 0xFF;
}

/*
returns tag, 18 most significant bits of address

32 - 6 - 8 = 18 bits
*/
uint32_t getTag(uint32_t address) { 
  return address >> 14;
}

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

/*********************** L1 cache *************************/

void initCache() { 
  initDram();
  initL1();
}

/* Initialize DRAM */
void initDram() {
  for (int i = 0; i < DRAM_SIZE; i++) {
    DRAM[i] = 0;
  }
}

/* Initialize L1 */
void initL1() {

  /* go through each line and set all properties to 0 */
  for (int i = 0; i < L1_LINES; i++) {
    SimpleCache.l1.line[i].Valid = 0;
    SimpleCache.l1.line[i].Dirty = 0;
    SimpleCache.l1.line[i].Tag = 0;


    /* set all words to 0 */
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      SimpleCache.l1.line[i].Data[j] = 0;
    }
  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t blockOffset = getBlockOffset(address);
  uint32_t lineIndex = getLineIndex(address);
  uint32_t tag = getTag(address);

  /* HIT, if line is valid and tag matches */
  if(SimpleCache.l1.line[lineIndex].Valid && SimpleCache.l1.line[lineIndex].Tag == tag) {
    if (mode == MODE_READ) {
      memcpy(data, &(SimpleCache.l1.line[lineIndex].Data[blockOffset]), WORD_SIZE);

      time += L1_READ_TIME;
    }
    if (mode == MODE_WRITE) {
      memcpy(&(SimpleCache.l1.line[lineIndex].Data[blockOffset]), data, WORD_SIZE);

      time += L1_WRITE_TIME;
      SimpleCache.l1.line[lineIndex].Dirty = 1; // why ?
    }
  } 
  /* MISS */
  else {
    if(mode == MODE_READ) {
      /* Read from dram */
      accessDRAM(address, data, MODE_READ);

      /* Write to cache */
      memcpy(&(SimpleCache.l1.line[lineIndex].Data[blockOffset]), data, WORD_SIZE);
      SimpleCache.l1.line[lineIndex].Valid = 1;
      SimpleCache.l1.line[lineIndex].Dirty = 0;
      SimpleCache.l1.line[lineIndex].Tag = tag;

      time += L1_READ_TIME;
    }

    if(mode == MODE_WRITE) {
      /* Write to dram */
      accessDRAM(address, data, MODE_WRITE);

      /* Write to cache */
      memcpy(&(SimpleCache.l1.line[lineIndex].Data[blockOffset]), data, WORD_SIZE);
      SimpleCache.l1.line[lineIndex].Valid = 1;
      SimpleCache.l1.line[lineIndex].Dirty = 0;
      SimpleCache.l1.line[lineIndex].Tag = tag;

      time += L1_WRITE_TIME;
    }
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
