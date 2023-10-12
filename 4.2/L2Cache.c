#include "L2Cache.h"

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
  initDRAM();
  initL1();
  initL2();
}

/* Initialize DRAM */
void initDRAM() {
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

/* Initialize L2 */
void initL2() {
  
    /* go through each line and set all properties to 0 */
    for (int i = 0; i < L2_LINES; i++) {
      SimpleCache.l2.line[i].Valid = 0;
      SimpleCache.l2.line[i].Dirty = 0;
      SimpleCache.l2.line[i].Tag = 0;

      for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
        SimpleCache.l2.line[i].Data[j] = 0;
      }
    }
}

/* Access L1 */
void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t blockOffset = getBlockOffset(address);
  uint32_t lineIndex = getLineIndex(address);
  uint32_t tag = getTag(address);

  CacheLine *Line = &SimpleCache.l1.line[lineIndex];

  /* HIT, if line is valid and tag matches */
  if(Line->Valid && Line->Tag == tag) {
    if (mode == MODE_READ) {
      memcpy(data, &(Line->Data[blockOffset]), WORD_SIZE);

      time += L1_READ_TIME;
    }
    if (mode == MODE_WRITE) {
      memcpy(&(Line->Data[blockOffset]), data, WORD_SIZE);

      /*Bit to alert cache was written to and hasnt updated memory*/
      Line->Dirty = 1;

      time += L1_WRITE_TIME;
    }
  } 
  /* MISS */
  else {
    /* Check if Dirty bit */
    /*
    To get whole block you start from start of block
    memory of start of block = address - block offset
    */
    if(Line->Dirty) {
      /* Write all block data to dram */
      accessL2(address - blockOffset, Line->Data, MODE_WRITE);
    }

    /* Get block of data from dram */
    /* 
    You need this for READ and WRITE 
    because if you write you first have to get whole block as well
    to after only write to certain offeset
    */
    accessL2(address - blockOffset, Line->Data, MODE_READ);

    Line->Valid = 1;
    Line->Tag = tag;

    if(mode == MODE_READ) {
      memcpy(data, &Line->Data[blockOffset], WORD_SIZE);

      Line->Dirty = 0;

      time += L1_READ_TIME;
    }

    if(mode == MODE_WRITE) {
      memcpy(&Line->Data[blockOffset], data, WORD_SIZE);

      Line->Dirty = 1;

      time += L1_WRITE_TIME;
    }
  }
}

/* Access L2 */
void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t blockOffset = getBlockOffset(address);
  uint32_t lineIndex = getLineIndex(address);
  uint32_t tag = getTag(address);

  CacheLine *Line = &SimpleCache.l2.line[lineIndex];

  /* HIT, if line is valid and tag matches */
  if(Line->Valid && Line->Tag == tag) {
    if (mode == MODE_READ) {
      memcpy(data, &(Line->Data[blockOffset]), WORD_SIZE);

      time += L2_READ_TIME;
    }
    if (mode == MODE_WRITE) {
      memcpy(&(Line->Data[blockOffset]), data, WORD_SIZE);

      /*Bit to alert cache was written to and hasnt updated memory*/
      Line->Dirty = 1;

      time += L2_WRITE_TIME;
    }
  } 
  /* MISS */
  else {
    /* Check if Dirty bit */
    /*
    To get whole block you start from start of block
    memory of start of block = address - block offset
    */
    if(Line->Dirty) {
      /* Write all block data to dram */
      accessDRAM(address - blockOffset, Line->Data, MODE_WRITE);
    }

    /* Get block of data from dram */
    /* 
    You need this for READ and WRITE 
    because if you write you first have to get whole block as well
    to after only write to certain offeset
    */
    accessDRAM(address - blockOffset, Line->Data, MODE_READ);

    Line->Valid = 1;
    Line->Tag = tag;

    if(mode == MODE_READ) {
      memcpy(data, &Line->Data[blockOffset], WORD_SIZE);

      Line->Dirty = 0;

      time += L2_READ_TIME;
    }

    if(mode == MODE_WRITE) {
      memcpy(&Line->Data[blockOffset], data, WORD_SIZE);

      Line->Dirty = 1;

      time += L2_WRITE_TIME;
    }
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
