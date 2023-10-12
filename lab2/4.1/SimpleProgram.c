
#include "L1Cache.h"
#include <stdio.h>

int main() {
    initCache(); // Initialize the L1 cache

    // Define addresses for cache fill and access
    uint32_t fillAddresses[] = {0, 64, 128, 192};
    uint32_t accessAddresses[] = {0, 64, 128, 192, 256};

    // Fill the cache with data
    for (int i = 0; i < sizeof(fillAddresses) / sizeof(uint32_t); i++) {
        uint8_t data = i + 10;
        int miss = write(fillAddresses[i], &data);
        if (miss) {
            printf("Cache miss on write at address %d\n", fillAddresses[i]);
        } else {
            printf("Wrote data %d to address %d\n", data, fillAddresses[i]);
        }
    }

    // Access the cache to check for cache misses
    for (int i = 0; i < sizeof(accessAddresses) / sizeof(uint32_t); i++) {
        uint8_t data;
        int miss = read(accessAddresses[i], &data);
        if (miss) {
            printf("Cache miss on read at address %d\n", accessAddresses[i]);
        } else {
            printf("Read data from address %d: %d\n", accessAddresses[i], data);
        }
    }

    return 0;
}
