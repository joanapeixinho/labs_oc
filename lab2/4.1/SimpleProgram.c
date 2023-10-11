#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <papi.h>
#include "L1Cache.h" 

#define CACHE_MIN (8 * 1024)
#define CACHE_MAX (64 * 1024)
#define N_REPETITIONS (200)

// Função para manipular erros do PAPI
void handle_error(char *outstring) {
    fprintf(stderr, "Error in PAPI function call %s\n", outstring);
    exit(EXIT_FAILURE);
}

int main() {
    uint8_t *array = calloc(CACHE_MAX, sizeof(uint8_t));
    if (array == NULL) {
        fprintf(stderr, "[ERR]: failed to allocate %d B: %s\n", CACHE_MAX, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Create the Event Set */
    int EventSet = PAPI_NULL;
    if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
        handle_error("create_eventset");
    }

    // Adicione eventos PAPI relevantes para sua cache L1
    if (PAPI_add_event(EventSet, PAPI_L1_DCM) != PAPI_OK) {
        handle_error("add_event");
    }

    // Inicialize sua cache L1
    initCache();

    for (size_t cache_size = CACHE_MIN; cache_size <= CACHE_MAX; cache_size = cache_size * 2) {
        for (size_t stride = 1; stride <= cache_size / 2; stride = stride * 2) {
            size_t const limit = cache_size - stride + 1;

            /* Reset the counting events in the Event Set */
            if (PAPI_reset(EventSet) != PAPI_OK) {
                handle_error("reset");
            }

            /* Read the counting of events in the Event Set */
            long long values[1];
            if (PAPI_read(EventSet, values) != PAPI_OK) {
                handle_error("read");
            }

            /* Start counting events in the Event Set */
            if (PAPI_start(EventSet) != PAPI_OK) {
                handle_error("start");
            }

            // Sua operação de cache simulada
            size_t n_iterations = 0;
            for (size_t repeat = 0; repeat <= N_REPETITIONS * stride; repeat++) {
                for (size_t index = 0; index < limit; index += stride, n_iterations++) {
                    uint8_t value;
                    read(index, &value);
                    write(index, &value);
                }
            }

            /* Stop the counting of events in the Event Set */
            if (PAPI_stop(EventSet, values) != PAPI_OK) {
                handle_error("stop");
            }

            /* Calcule métricas relacionadas à sua cache L1 a partir dos valores coletados */
            float const avg_misses = (float)(values[0]) / n_iterations;
            fprintf(stdout, "cache_size=%zu\tstride=%zu\tavg_misses=%f\n", cache_size, stride, avg_misses);
        }
    }

    return 0;
}
