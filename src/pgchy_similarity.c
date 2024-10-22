#include "postgres.h"
#include "fmgr.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <libpopcnt.h>

#include <includes/pgchy_similarity.h>


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


int popcount_and(const uint8_t *arr1, const uint8_t *arr2) {
    const __m256i lookup = _mm256_setr_epi8(
        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4,

        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4
    );
    const __m256i low_mask = _mm256_set1_epi8(0x0f);

    // Process 32 bytes at a time using AVX2
    __m256i local = _mm256_setzero_si256();
    for (size_t i = 0; i < 256; i += 32) {
        // Load 32 bytes from each array into AVX2 registers
        const __m256i vec1 = _mm256_loadu_si256((const __m256i *)(arr1 + i));
        const __m256i vec2 = _mm256_loadu_si256((const __m256i *)(arr2 + i));

        // Perform bitwise AND operation on the 32-byte chunks
        const __m256i vec = _mm256_and_si256(vec1, vec2);

        const __m256i lo  = _mm256_and_si256(vec, low_mask);
        const __m256i hi  = _mm256_and_si256(_mm256_srli_epi16(vec, 4), low_mask);
        const __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);
        const __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);
        local = _mm256_add_epi8(local, popcnt1);
        local = _mm256_add_epi8(local, popcnt2);
    }

    local = _mm256_sad_epu8(local, _mm256_setzero_si256());

    uint64_t result = 0;
    result += (uint64_t)(_mm256_extract_epi64(local, 0));
    result += (uint64_t)(_mm256_extract_epi64(local, 1));
    result += (uint64_t)(_mm256_extract_epi64(local, 2));
    result += (uint64_t)(_mm256_extract_epi64(local, 3));
    return result;
}


int check_popcounts(Fingerprint* fingerprint1, Fingerprint* fingerprint2, float threshold) {
    uint16_t popcount1 = fingerprint1->popcount;
    uint16_t popcount2 = fingerprint2->popcount;
    if (popcount1 > popcount2) {
        return (popcount2 < (threshold * popcount1)) ? 0 : 1;
    }
    return (popcount1 < (threshold * popcount2)) ? 0 : 1;
}


int similarity_256(Fingerprint* fingerprint1, Fingerprint* fingerprint2, float threshold) {
    uint64_t intersection;
    uint64_t union_count;

    if (check_popcounts(fingerprint1, fingerprint2, threshold) == 0) {
        return 0;
    }

    intersection = popcount_and(fingerprint1->arr, fingerprint2->arr);
    union_count = fingerprint1->popcount + fingerprint2->popcount - intersection;

    return ((float)intersection / (float)union_count >= threshold) ? 1 : 0;
}


PGDLLEXPORT Datum is_tanimoto_similar(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(is_tanimoto_similar);


Datum is_tanimoto_similar(PG_FUNCTION_ARGS) {
    Fingerprint* query_fingerprint;
    Fingerprint* mol_fingerprint;
    bool result;

    bytea* query_bytes = PG_GETARG_BYTEA_P(0);
    bytea* mol_bytes = PG_GETARG_BYTEA_P(1);
    float threshold = PG_GETARG_FLOAT4(2);

    query_fingerprint = (Fingerprint*) VARDATA(query_bytes);
    mol_fingerprint = (Fingerprint*) VARDATA(mol_bytes);

    result = similarity_256(query_fingerprint, mol_fingerprint, threshold) > 0;

    PG_RETURN_BOOL(result);
}
