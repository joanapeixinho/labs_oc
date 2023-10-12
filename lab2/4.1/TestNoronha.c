#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>
#include "SimpleCache.h"


#define CACHE_MIN (8 * 1024)
#define CACHE_MAX (64 * 1024)
#define N_REPETITIONS (200)

void handle_error(char *outstring);

int main() {

    for (size_t cache_size = CACHE_MIN; cache_size <= CACHE_MAX;
         cache_size = cache_size * 2) {
        for (size_t stride = 1; stride <= cache_size / 2; stride = stride * 2) {
            size_t const limit = cache_size - stride + 1;


            /************/
            uint8_t x;
            size_t n_iterations = 0;
            long long int misses = 0;
            for (size_t repeat = 0; repeat <= N_REPETITIONS * stride; repeat++) {
                for (size_t index = 0; index < limit; index += stride, n_iterations++) {
                    read(index, &x);
                }
                misses += getMisses();
                resetMisses();
                
            }
            /************/


            /************/
            float const avg_misses = (float)misses / n_iterations;
            fprintf(stdout,
                    "cache_size=%zu\tstride=%zu\tavg_misses=%f\n",
                    cache_size, stride, avg_misses);
        }
    }

    return 0;
}