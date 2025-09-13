#ifndef UNITY_H
#define UNITY_H

#include <stddef.h>
#include <stdint.h>

// Unity Assert Functions
void TEST_ASSERT_EQUAL(int expected, int actual);
void TEST_ASSERT_EQUAL_INT(int expected, int actual);
void TEST_ASSERT_EQUAL_UINT8(uint8_t expected, uint8_t actual);
void TEST_ASSERT_EQUAL_STRING(const char* expected, const char* actual);

// Unity Test Framework Functions
void UnityBegin(const char* file);
int UnityEnd(void);

// Unity Test Running Macros
#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END() UnityEnd()
#define RUN_TEST(test) test()

#endif /* UNITY_H */