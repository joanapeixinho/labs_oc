#ifndef CACHE_H
#define CACHE_H

#define WORD_SIZE (4)                 // in bytes, i.e 32 bit words
#define WORD_OFFSET_BITS (2)         // 4 bytes = 2^2 
#define BLOCK_SIZE (16 * WORD_SIZE)    // in bytes
#define BLOCK_OFFSET_BITS (4)       // 16 bytes = 2^4
#define DRAM_SIZE (1024 * BLOCK_SIZE) // in bytes
#define L1_SIZE (256 * BLOCK_SIZE)      // in bytes
#define L1_LINES (L1_SIZE / BLOCK_SIZE) // in bytes
#define L1_INDEX_BITS (8)            // 256 lines = 2^8
#define L2_SIZE (512 * BLOCK_SIZE)    // in bytes
#define L2_LINES (L2_SIZE / BLOCK_SIZE) // in bytes
#define L2_INDEX_BITS (9)            // 512 lines = 2^9
#define L2_2W_LINES (L2_SIZE / (2*BLOCK_SIZE))
#define L2_2W_INDEX_BITS (8)

#define MODE_READ 1
#define MODE_WRITE 0

#define DRAM_READ_TIME 100
#define DRAM_WRITE_TIME 50
#define L2_READ_TIME 10
#define L2_WRITE_TIME 5
#define L1_READ_TIME 1
#define L1_WRITE_TIME 1

#endif
