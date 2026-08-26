#ifndef DC166_RUNTIME_TEST_H
#define DC166_RUNTIME_TEST_H

typedef unsigned char test_u8;
typedef signed int test_i16;
typedef unsigned int test_u16;
typedef signed long test_i32;
typedef unsigned long test_u32;

#ifndef DC166_TEST_NO_FP
typedef union {
  float value;
  test_u16 words[2];
} test_f32;

typedef union {
  double value;
  test_u16 words[4];
} test_f64;
#endif

extern volatile test_u16 simulator_result;
void simulator_stop(void);

static void test_check(test_u16 id, test_u16 condition)
{
  if (!condition && simulator_result == 0U)
    simulator_result = id;
}

#ifndef DC166_TEST_NO_FP
static void test_check_f32(test_u16 id, test_f32 *value,
                           test_u16 high, test_u16 low)
{
  test_check(id, value->words[0] == high && value->words[1] == low);
}

static void test_check_f64(test_u16 id, test_f64 *value,
                           test_u16 w3, test_u16 w2,
                           test_u16 w1, test_u16 w0)
{
  test_check(id, value->words[0] == w3 && value->words[1] == w2 &&
                 value->words[2] == w1 && value->words[3] == w0);
}
#endif

#endif
