#include "L2_Assoc_Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache CacheL2 = {0};

//delete
int L1_anum = 0, L2_anum = 0, DRAM_anum = 0;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {
  STATUS("DRAM access %d | Address: %d | Mode: %s| Time before: %d", DRAM_anum, address, (mode) ? "Read" : "Write", time);

  if (address >= DRAM_SIZE - WORD_SIZE + 1) {
    STATUS("Error in accessDRAM: Address out of bounds");
    exit(-1);
  }

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
    STATUS("DRAM access %d | Read | Time after: %d", DRAM_anum++, time);
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
    STATUS("DRAM access %d | Write | Time after: %d", DRAM_anum++, time);
  }
}

/*********************** L1 e L2 caches *************************/

void initCache() {
  STATUS("Initializing Cache");
  for (int i = 0; i < L1_LINES; i++) {
    CacheL2.linesL1[i].Valid = 0;
    CacheL2.linesL1[i].Dirty = 0;
    CacheL2.linesL1[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      CacheL2.linesL1[i].Data[j] = 0;
    }
  }
  for (int i = 0; i < L2_ASSOC_LINES; i++) {
    for (int k = 0; k < L2_ASSOCIATIVITY; k++) {
      CacheL2.linesL2[i][k].Valid = 0;
      CacheL2.linesL2[i][k].Dirty = 0;
      CacheL2.linesL2[i][k].Tag = 0;
      CacheL2.linesL2[i][k].LRU_idx = k;
      for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
        CacheL2.linesL2[i][k].Data[j] = 0;
      }
    }
  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  STATUS("L1 Access %d | Address: %d | Mode: %s| Time before: %d", L1_anum, address, (mode) ? "Read" : "Write", time);

  uint32_t index, Tag, offset;
  uint32_t miss = 0;
  uint8_t TempBlock[BLOCK_SIZE];

  Tag = address >> (L1_INDEX_BITS + OFFSET_BITS);
  index = (address & ((1 << (L1_INDEX_BITS + OFFSET_BITS)) - 1)) >> (OFFSET_BITS); 
  offset = (address & ((1 << OFFSET_BITS) - 1));

  CacheLine *Line = &CacheL2.linesL1[index];


  /*MISS*/
  if (!Line->Valid || Line->Tag != Tag) {         // if block not present - miss
    STATUS("L1 Miss: Getting new block from L2");
    accessL2(address, TempBlock, MODE_READ); // get new block from L2
    if ((Line->Valid) && (Line->Dirty)) { // line has dirty block
      STATUS("L1 Write Back: Write old block to L2");
      accessL2(address, &(Line->Data[offset]), MODE_WRITE); // then write back old block (Write Back policy)
    }

    STATUS("L1 Write Allocate: Write new block to L1");
    memcpy(&(Line->Data[offset]), TempBlock, BLOCK_SIZE); // copy new block to cache line
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  } // if miss, then replaced with the correct block


  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(Line->Data[offset]), WORD_SIZE);
    time += L1_READ_TIME;
    STATUS("L1 Access %d | Read | Time after: %d", L1_anum++, time);
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line->Data[offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
    STATUS("L1 Access %d | Write | Time after: %d", L1_anum++, time);
  }
  return miss;
}

void use_block(CacheLine *line, int block_idx) {
  for (int i = 0; i < L2_ASSOCIATIVITY; i++) {
    if (line[i].LRU_idx > line[block_idx].LRU_idx) {
      line[i].LRU_idx--;
    }
  }
  line[block_idx].LRU_idx = L2_ASSOCIATIVITY - 1;
}

int get_LRU_block(CacheLine *line) {
  for (int i = 0; i < L2_ASSOCIATIVITY; i++) {
    if (line[i].LRU_idx == 0) {
      return i;
    }
  }
  return -1;
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  STATUS("L2 Access %d | Address: %d | Mode: %s| Time before: %d", L2_anum, address, (mode) ? "Read" : "Write", time);
  uint32_t index, Tag, MemAddress, offset;
  uint32_t miss = 0;
  uint8_t TempBlock[BLOCK_SIZE];

  Tag = address >> (L2_ASSOC_INDEX_BITS + OFFSET_BITS);
  index = (address & ((1 << (L2_ASSOC_INDEX_BITS + OFFSET_BITS)) - 1)) >> (OFFSET_BITS); 
  offset = (address & ((1 << OFFSET_BITS) - 1));

  CacheLine *Line = CacheL2.linesL2[index];

  MemAddress = address - offset; // get address of the block in memory 

  STATUS("L2: Searching for the right block in line %d", index);
  /* access Cache*/
  int block_idx = -1;
  for (int i = 0; i < L2_ASSOCIATIVITY; i++) {
    if (Line[i].Valid && Line[i].Tag == Tag) {
      STATUS("L2: Found block %d", i);
      block_idx = i;
      break;
    }
    else if (!Line[i].Valid && block_idx == -1) {
      block_idx = i;
    }
  }

  /*MISS*/
  if (block_idx == -1 || !Line[block_idx].Valid) {         // if block not present - miss
    
    if (block_idx == -1) {
      block_idx = get_LRU_block(Line);
      STATUS("L2 Miss: No empty blocks -> Getting new block from memory");
    } else {
      STATUS("L2 Miss: Found empty block %d -> Getting new block from memory", block_idx);
    }

    accessDRAM(MemAddress, TempBlock, MODE_READ); // get new block from L2
    if ((Line[block_idx].Valid) && (Line[block_idx].Dirty)) { // line has dirty block
      STATUS("L2 Write Back: Write old block to memory");
      accessDRAM(MemAddress, &(Line[block_idx].Data[offset]), MODE_WRITE); // then write back old block
    }
    
    STATUS("L2 Write Allocate: Write new block to L2");
    memcpy(&(Line[block_idx].Data[offset]), TempBlock, BLOCK_SIZE); // copy new block to cache line
    Line[block_idx].Valid = 1;
    Line[block_idx].Tag = Tag;
    Line[block_idx].Dirty = 0;
  }
  
  STATUS("Setting last LRU block %d of line %d", block_idx, index);
  use_block(Line, block_idx);

  if (mode == MODE_READ) {    // read data from cache line
    memcpy(data, &(Line[block_idx].Data[offset]), WORD_SIZE);
    time += L2_READ_TIME;
    STATUS("L2 Access %d | Read | Time after: %d", L2_anum++, time);
  }

  if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&(Line[block_idx].Data[offset]), data, WORD_SIZE);
    time += L2_WRITE_TIME;
    Line[block_idx].Dirty = 1;
    STATUS("L2 Access %d | Write | Time after: %d", L2_anum++, time);
  }
  return miss;
}

void read(uint32_t address, uint8_t *data) {
  STATUS("Request to read address %d", address);
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  STATUS("Request to write address %d", address);
  accessL1(address, data, MODE_WRITE);
}
