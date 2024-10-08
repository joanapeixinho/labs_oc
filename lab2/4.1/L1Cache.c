#include "L1Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache CacheL1 = {0};

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
    CacheL1.lines[i].Valid = 0;
    CacheL1.lines[i].Dirty = 0;
    CacheL1.lines[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      CacheL1.lines[i].Data[j] = 0;
    }
  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, MemAddress, offset;
  uint8_t TempBlock[BLOCK_SIZE];

  Tag = address >> (L1_INDEX_BITS + OFFSET_BITS);
  index = (address & ((1 << (L1_INDEX_BITS + OFFSET_BITS)) - 1)) >> (OFFSET_BITS); 
  offset = (address & ((1 << OFFSET_BITS) - 1));

  CacheLine *Line = &CacheL1.lines[index];

  MemAddress = (address >> OFFSET_BITS) << OFFSET_BITS; // get address of the block in memory 

  /* access Cache*/


/*MISS*/
  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM
    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      uint32_t MemAddress_old = (Line->Tag << L1_INDEX_BITS || index) << OFFSET_BITS;
      accessDRAM(MemAddress_old, Line->Data, MODE_WRITE); // then write back old block (Write Back policy)
    }

    memcpy(Line->Data, TempBlock, BLOCK_SIZE); // copy new block to cache line
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

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
