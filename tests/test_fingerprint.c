#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <libpopcnt.h>
#include <includes/pgchy_similarity.h>



static void test_similarity_256(void **state) {
    (void) state;

    Fingerprint fp1 = {
        .arr = {0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55,
                0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55,
                0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55, 0xFF, 0x00, 0xAA, 0x55,
                0xFF, 0x00, 0xAA, 0x55},
        .popcount = popcnt((const void *) fp1.arr, SIM_VEC_SIZE)
    };

    Fingerprint fp2 = {
        .arr = {0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05,
                0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05,
                0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05, 0x0F, 0xF0, 0x0A, 0x05,
                0x0F, 0xF0, 0x0A, 0x05},
        .popcount = popcnt((const void *) fp2.arr, SIM_VEC_SIZE)
    };

    float threshold = 0.2f;

    int similarity = similarity_256(&fp1, &fp2, threshold);
    int expected_similarity = 0;

    assert_int_equal(similarity, expected_similarity);
}

static void test_create_fingerprint(void **state) {
    (void) state;

    uint8_t test_data[SIM_VEC_SIZE + 2];
    test_data[0] = 0x12;
    test_data[1] = 0x34;
    for (int i = 0; i < SIM_VEC_SIZE; i++) {
        test_data[i + 2] = (uint8_t) i;
    }

    Fingerprint *fp = (Fingerprint *) test_data;

    assert_int_equal(fp->popcount, 0x3412);
    for (int i = 0; i < SIM_VEC_SIZE; i++) {
        assert_int_equal(fp->arr[i], (uint8_t) i);
    }
}


static void test_decompress(void **state) {


}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_and_vector_256),
        cmocka_unit_test(test_similarity_256),
        cmocka_unit_test(test_create_fingerprint),
        cmocka_unit_test(test_create_fingerprint),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
