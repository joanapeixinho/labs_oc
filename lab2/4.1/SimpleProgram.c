#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>

#define CACHE_MIN (8 * 1024)
#define CACHE_MAX (64 * 1024)
#define N_REPETITIONS (200)


int main() {

    uint8_t *array = calloc(CACHE_MAX, sizeof(uint8_t));
    if (array == NULL) {
        fprintf(stderr, "[ERR]: failed to allocate %d B: %s\n", CACHE_MAX,
                strerror(errno));
        exit(EXIT_FAILURE);
    }



    for (size_t cache_size = CACHE_MIN; cache_size <= CACHE_MAX;
         cache_size = cache_size * 2) {
        for (size_t stride = 1; stride <= cache_size / 2; stride = stride * 2) {
            size_t const limit = cache_size - stride + 1;


      

            /************************************/
            size_t n_iterations = 0;
            for (size_t repeat = 0; repeat <= N_REPETITIONS * stride; repeat++) {
                for (size_t index = 0; index < limit; index += stride, n_iterations++) {
                    array[index] = array[index] + 1;
                }
            }

            /************************************/
            float const avg_misses = (float)(values[0]) / n_iterations;
            float const avg_time = (float)(end_usec - start_usec) / n_iterations;
            float const avg_cycles = (float)(end_cycles - start_cycles) / n_iterations;
            fprintf(stdout,
                    "cache_size=%zu\tstride=%zu\tavg_misses=%f\tavg_cycl_time=%f\n",
                    cache_size, stride, avg_misses, avg_time);
        }
    }

    return 0;
}

void handle_error(char *outstring) {
    printf("Error in PAPI function call %s\n", outstring);
    PAPI_perror("PAPI Error");
    exit(EXIT_FAILURE);
}
