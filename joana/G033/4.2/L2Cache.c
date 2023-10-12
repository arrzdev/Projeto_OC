#include "L2Cache.h"

uint8_t L1Cache[L1_SIZE];
uint8_t L2Cache[L2_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache cache;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode)
{

    if (address >= DRAM_SIZE - WORD_SIZE + 1)
        exit(-1);

    if (mode == MODE_READ)
    {
        memcpy(data, &(DRAM[address]), BLOCK_SIZE);
        time += DRAM_READ_TIME;
    }

    if (mode == MODE_WRITE)
    {
        memcpy(&(DRAM[address]), data, BLOCK_SIZE);
        time += DRAM_WRITE_TIME;
    }
}

/*********************** Initialize the cache *************************/
void initCache()
{
    for (int i = 0; i < L1_SIZE / BLOCK_SIZE; i += 1)
    {
        cache.l1.lines[i].Valid = 0;
        cache.l1.lines[i].Dirty = 0;
        cache.l1.lines[i].Tag = 0;
        for (int j = 0; j < BLOCK_SIZE; j += WORD_SIZE)
        {
            cache.l1.lines[i].Data[j] = 0;
        }
    }
    cache.l1.init = 1;

    for (int i = 0; i < L2_SIZE / BLOCK_SIZE; i += 1)
    {
        cache.l2.lines[i].Valid = 0;
        cache.l2.lines[i].Dirty = 0;
        cache.l2.lines[i].Tag = 0;
        for (int j = 0; j < BLOCK_SIZE; j += WORD_SIZE)
        {
            cache.l2.lines[i].Data[j] = 0;
        }
    }
    cache.l2.init = 1;

    cache.init = 1;
}

/*********************** L2 cache *************************/
void accessL2(uint32_t address, uint8_t *data, uint32_t mode)
{
    uint32_t index, tag, offset;

    index = (address / BLOCK_SIZE) % (L2_SIZE / BLOCK_SIZE);
    tag = address / ((L2_SIZE / BLOCK_SIZE) * BLOCK_SIZE);
    offset = address % BLOCK_SIZE;

    CacheLine *Line = &cache.l2.lines[index];

    /* access Cache*/

    if (Line->Valid && Line->Tag == tag)
    {
        if (mode == MODE_READ)
        { // read data from cache line
            memcpy(data, &(Line->Data[offset]), WORD_SIZE);
            time += L2_READ_TIME;
        }

        if (mode == MODE_WRITE)
        { // write data from cache line
            memcpy(&(Line->Data[offset]), data, WORD_SIZE);
            time += L2_WRITE_TIME;
            Line->Dirty = 1;
        }
    }
    else
    {
        if (Line->Dirty)
        {                                                         // line has dirty block
            accessDRAM(address - offset, Line->Data, MODE_WRITE); // then write back old block
        }
        accessDRAM(address - offset, Line->Data, MODE_READ); // get new block from DRAM

        Line->Valid = 1;
        Line->Tag = tag;

        if (mode == MODE_READ)
        { // read data from cache line
            memcpy(data, &(Line->Data[offset]), WORD_SIZE);
            time += L2_READ_TIME;
            Line->Dirty = 0;
        }

        if (mode == MODE_WRITE)
        { // write data from cache line
            memcpy(&(Line->Data[offset]), data, WORD_SIZE);
            time += L2_WRITE_TIME;
            Line->Dirty = 1;
        }
    }
}

/*********************** L1 cache *************************/
void accessL1(uint32_t address, uint8_t *data, uint32_t mode)
{

    uint32_t index, tag, offset;

    index = (address / BLOCK_SIZE) % (L1_SIZE / BLOCK_SIZE);
    tag = address / ((L1_SIZE / BLOCK_SIZE) * BLOCK_SIZE);
    offset = address % BLOCK_SIZE;

    CacheLine *Line = &cache.l1.lines[index];

    /* access Cache*/

    if (Line->Valid && Line->Tag == tag)
    {
        if (mode == MODE_READ)
        { // read data from cache line
            memcpy(data, &(Line->Data[offset]), WORD_SIZE);
            time += L1_READ_TIME;
        }

        if (mode == MODE_WRITE)
        { // write data from cache line
            memcpy(&(Line->Data[offset]), data, WORD_SIZE);
            time += L1_WRITE_TIME;
            Line->Dirty = 1;
        }
    }
    else
    {
        if (Line->Dirty)
        {                                                       // line has dirty block
            accessL2(address - offset, Line->Data, MODE_WRITE); // then write back old block
        }
        accessL2(address - offset, Line->Data, MODE_READ); // get new block from DRAM

        Line->Valid = 1;
        Line->Tag = tag;

        if (mode == MODE_READ)
        { // read data from cache line
            memcpy(data, &(Line->Data[offset]), WORD_SIZE);
            time += L1_READ_TIME;
            Line->Dirty = 0;
        }

        if (mode == MODE_WRITE)
        { // write data from cache line
            memcpy(&(Line->Data[offset]), data, WORD_SIZE);
            time += L1_WRITE_TIME;
            Line->Dirty = 1;
        }
    }
}

void read(uint32_t address, uint8_t *data)
{
    accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data)
{
    accessL1(address, data, MODE_WRITE);
}
