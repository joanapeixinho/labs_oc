#include "L1Cache.h"

uint8_t L1Cache[L1_SIZE];
uint8_t L2Cache[L2_SIZE];
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
  }
  CacheL1.init = 1;
}



void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, MemAddress, word_index, cache_index;
  uint8_t TempBlock[BLOCK_SIZE];

  /* init cache */
  printf("init: %d\n", CacheL1.init);
  if (CacheL1.init == 0) {
    initCache();
  }

  Tag = address >> (L1_INDEX_BITS + BLOCK_OFFSET_BITS + WORD_OFFSET_BITS);
  index = (address & 0x3FFF) >> (BLOCK_OFFSET_BITS + WORD_OFFSET_BITS);
  word_index = (address & 0x3F) >> WORD_OFFSET_BITS;

  cache_index = index * BLOCK_SIZE + word_index;

  CacheLine *Line = &CacheL1.lines[index];

  MemAddress = address >> WORD_OFFSET_BITS; // again this....!
  MemAddress = MemAddress << WORD_OFFSET_BITS; // address of the block in memory  

  /* access Cache*/

  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from DRAM

    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      MemAddress = ((Line->Tag << L1_INDEX_BITS | index) << BLOCK_OFFSET_BITS | word_index) << WORD_OFFSET_BITS; // get address of the block in memory
      
      accessDRAM(MemAddress, &(L1Cache[cache_index]), MODE_WRITE); // then write back old block
    }

    memcpy(&(L1Cache[cache_index]), TempBlock, BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block


//we dont know how to handle this part:
  if (mode == MODE_READ) {    // read data from cache line
    if (0 == (address % WORD_SIZE)) { // even word on block
      memcpy(data, &(L1Cache[cache_index]), WORD_SIZE);
    } else { // odd word on block
      memcpy(data, &(L1Cache[cache_index + WORD_SIZE]), WORD_SIZE);
    }
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write data from cache line
    if (!(address % WORD_SIZE)) {   // even word on block
      memcpy(&(L1Cache[cache_index]), data, WORD_SIZE);
    } else { // odd word on block
      memcpy(&(L1Cache[cache_index + WORD_SIZE ]), data, WORD_SIZE);
    }
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
