#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>
#include "../Cache.h"

#define CACHE_MIN (1024)
#define CACHE_MAX (DRAM_SIZE)
#define N_REPETITIONS (200)


int main() {

    int avg_time = 0;
    uint8_t *array = calloc(CACHE_MAX, sizeof(uint8_t));
    int misses = 0;
    if (array == NULL) {
        fprintf(stderr, "[ERR]: failed to allocate %d B: %s\n", CACHE_MAX,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

  /*warm up cache*/
    for (size_t index = 0; index < CACHE_MAX; index += 4) {
        read(index, array+index);
        array[index]++;
        write(index, array+index);
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
                    "cache_size=%zu\tstride=%zu\tavg_misses=%f\tavg_cycl_time=%d\n",
                    cache_size, stride, avg_misses, avg_time);
        }
    }

    return 0;
}

void handle_error(char *outstring) {
    fprintf(stderr, "%s\n", outstring);
    exit(EXIT_FAILURE);
}
