#include "../SimpleCache.h"

int main() {
  // set seed for random number generator
  srand(0);

  int clock1, value;

  // Initialize cache
  resetTime();
  initCache();

  // Sequential writes and reads to cause cache conflicts and evictions
  printf("Sequential writes and reads to cause cache conflicts and evictions:\n");
  for (int i = 0; i < L1_LINES * L2_ASSOCIATIVITY; i++) {
    write(i * WORD_SIZE, (unsigned char *)(&i));
    clock1 = getTime();
    printf("Write; Address %d; Value %d; Time %d\n", i * WORD_SIZE, i, clock1);
  }

  for (int i = 0; i < L1_LINES * L2_ASSOCIATIVITY; i++) {
    read(i * WORD_SIZE, (unsigned char *)(&value));
    clock1 = getTime();
    printf("Read; Address %d; Value %d; Time %d\n", i * WORD_SIZE, value, clock1);
  }

  // Random accesses to the cache causing evictions
  printf("\nRandom accesses causing evictions:\n");
  for (int i = 0; i < 2 * L1_LINES * L2_ASSOCIATIVITY; i++) {
    int address = rand() % (DRAM_SIZE / 4);
    address = address - address % WORD_SIZE;
    int mode = rand() % 2;
    if (mode == MODE_READ) {
      read(address, (unsigned char *)(&value));
      clock1 = getTime();
      printf("Read; Address %d; Value %d; Time %d\n", address, value, clock1);
    } else {
      write(address, (unsigned char *)(&address));
      clock1 = getTime();
      printf("Write; Address %d; Value %d; Time %d\n", address, address, clock1);
    }
  }

  return 0;
}
