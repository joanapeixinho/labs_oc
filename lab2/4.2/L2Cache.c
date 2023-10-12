#include "L2Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache CacheL2 = {0};

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
  for (int i = 0; i < L1_LINES; i++) {
    CacheL2.linesL1[i].Valid = 0;
    CacheL2.linesL1[i].Dirty = 0;
    CacheL2.linesL1[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      CacheL2.linesL1[i].Data[j] = 0;
    }
  }
  for (int i = 0; i < L2_LINES; i++) {
    CacheL2.linesL2[i].Valid = 0;
    CacheL2.linesL2[i].Dirty = 0;
    CacheL2.linesL2[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      CacheL2.linesL2[i].Data[j] = 0;
    }
  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, offset, L2Address;
  uint8_t TempBlock[BLOCK_SIZE];

  Tag = address >> (L1_INDEX_BITS + OFFSET_BITS);
  index = (address & ((1 << (L1_INDEX_BITS + OFFSET_BITS)) - 1)) >> (OFFSET_BITS); 
  offset = (address & ((1 << OFFSET_BITS) - 1));

  CacheLine *Line = &CacheL2.linesL1[index];

  L2Address = address - offset;

  /*MISS*/
  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessL2(address, TempBlock, MODE_READ); // get new block from L2
    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      accessL2(L2Address, &(Line->Data[offset]), MODE_WRITE); // then write back old block
    }

    memcpy(&(Line->Data[offset]), TempBlock, BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block


  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(Line->Data[offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, Tag, MemAddress, offset;
  uint8_t TempBlock[BLOCK_SIZE];

  Tag = address >> (L2_INDEX_BITS + OFFSET_BITS);
  index = (address & ((1 << (L2_INDEX_BITS + OFFSET_BITS)) - 1)) >> (OFFSET_BITS); 
  offset = (address & ((1 << OFFSET_BITS) - 1));

  CacheLine *Line = &CacheL2.linesL2[index];

  MemAddress = address - offset; // get address of the block in memory 

  /* access Cache*/

  /*MISS*/
  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from L2
    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      uint32_t MemAddress_old = (Line->Tag << L2_INDEX_BITS || index) << OFFSET_BITS;
      accessDRAM(MemAddress_old, &(Line->Data[offset]), MODE_WRITE); // then write back old block (Write Back policy)
    }

    memcpy(&(Line->Data[offset]), TempBlock, BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block

  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(Line->Data[offset]), WORD_SIZE);
    time += L2_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[offset]), data, WORD_SIZE);
    time += L2_WRITE_TIME;
    Line->Dirty = 1;
  }
}


void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
