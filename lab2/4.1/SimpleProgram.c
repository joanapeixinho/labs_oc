#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>
#include "../Cache.h"

#define CACHE_MIN (8 * 1024)
#define CACHE_MAX (DRAM_SIZE - WORD_SIZE + 1)
#define N_REPETITIONS (200)


int main() {

    uint8_t *array = calloc(CACHE_MAX, sizeof(uint8_t));
    int misses = 0;
    if (array == NULL) {
        fprintf(stderr, "[ERR]: failed to allocate %d B: %s\n", CACHE_MAX,
                strerror(errno));
        exit(EXIT_FAILURE);
    }


    for (size_t cache_size = CACHE_MIN; cache_size <= CACHE_MAX;
         cache_size = cache_size * 2) {
        for (size_t stride = 1; stride <= cache_size / 2; stride = stride * 2) {
            size_t const limit = cache_size - stride + 1;

            size_t n_iterations = 0;
            for (size_t repeat = 0; repeat <= N_REPETITIONS * stride; repeat++) {
                for (size_t index = 0; index < limit; index += stride, n_iterations++) {
                      misses += read(index*4, array+index*4);
                      array[index*4]++;
                      misses += write(index*4, array+index*4);

                }
            }

            float const avg_misses = misses / n_iterations;
            fprintf(stdout,
                    "cache_size=%zu\tstride=%zu\tavg_misses=%f\t\n",
                    cache_size, stride, avg_misses);
        }
    }

    return 0;
}

void handle_error(char *outstring) {
    fprintf(stderr, "%s\n", outstring);
    exit(EXIT_FAILURE);
}
