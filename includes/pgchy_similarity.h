#include "postgres.h"
#include <stdint.h>
#include <stddef.h>


#define SIM_VEC_SIZE 256
#define SIM_VEC_SIZE_INT 64


typedef struct {
    uint16_t popcount;
    uint8_t arr[SIM_VEC_SIZE];
} Fingerprint;


int popcount_and(const uint8_t *arr1, const uint8_t *arr2);

bool check_popcounts(uint16_t popcount1, uint16_t popcount2, float threshold);

bool similarity_256(Fingerprint* fingerprint1, Fingerprint* fingerprint2, float threshold);

Datum is_tanimoto_similar(PG_FUNCTION_ARGS);
