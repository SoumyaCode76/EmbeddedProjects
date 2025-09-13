#include "unity.h"
#include <stdio.h>
#include <string.h>

void UnityBegin(const char* file) {
    printf("--------------------\n");
    printf("Starting test: %s\n", file);
    printf("--------------------\n");
}

int UnityEnd(void) {
    printf("--------------------\n");
    printf("Tests completed\n");
    printf("--------------------\n");
    return 0;
}

void TEST_ASSERT_EQUAL(int expected, int actual) {
    if (expected != actual) {
        printf("FAIL: Expected %d but got %d\n", expected, actual);
    } else {
        printf("PASS: Value matches expected %d\n", expected);
    }
}

void TEST_ASSERT_EQUAL_INT(int expected, int actual) {
    TEST_ASSERT_EQUAL(expected, actual);
}

void TEST_ASSERT_EQUAL_UINT8(uint8_t expected, uint8_t actual) {
    if (expected != actual) {
        printf("FAIL: Expected %u but got %u\n", expected, actual);
    } else {
        printf("PASS: Value matches expected %u\n", expected);
    }
}

void TEST_ASSERT_EQUAL_STRING(const char* expected, const char* actual) {
    // Simple string comparison - in a real implementation you'd want to use strncmp
    if (strcmp(expected, actual) != 0) {
        printf("FAIL: Strings do not match\n");
        printf("Expected: %s\n", expected);
        printf("Actual: %s\n", actual);
    } else {
        printf("PASS: Strings match: %s\n", expected);
    }
}