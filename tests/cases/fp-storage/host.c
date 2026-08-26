#include "test.h"

_packed struct packed_double {
  test_u8 prefix;
  double value;
};

typedef struct {
  test_u16 w3;
  test_u16 w2;
  test_u16 w1;
  test_u16 w0;
} double_vector;

static const double_vector vectors[] = {
  {0x0000U, 0x0000U, 0x0000U, 0x0000U},
  {0x8000U, 0x0000U, 0x0000U, 0x0000U},
  {0x0000U, 0x0000U, 0x0000U, 0x0001U},
  {0x000fU, 0xffffU, 0xffffU, 0xffffU},
  {0x0010U, 0x0000U, 0x0000U, 0x0000U},
  {0x3ff0U, 0x0000U, 0x0000U, 0x0000U},
  {0xc004U, 0x0000U, 0x0000U, 0x0000U},
  {0x7ff8U, 0x1234U, 0x5678U, 0x9abcU},
};

static volatile test_f64 _near near_source;
static volatile test_f64 _near near_destination;
static volatile test_f64 _far far_source;
static volatile test_f64 _far far_destination;
static volatile test_f64 _huge huge_source;
static volatile test_f64 _huge huge_destination;
static _packed struct packed_double _near packed_near_source;
static _packed struct packed_double _near packed_near_destination;
static _packed struct packed_double _far packed_far_source;
static _packed struct packed_double _far packed_far_destination;
static _packed struct packed_double _huge packed_huge_source;
static _packed struct packed_double _huge packed_huge_destination;

#define DEFINE_VALUE_ACCESS(NAME, QUALIFIER)                                 \
static void set_value_##NAME(volatile test_f64 QUALIFIER *value,             \
                             const double_vector *vector)                    \
{                                                                            \
  value->words[0] = vector->w3;                                              \
  value->words[1] = vector->w2;                                              \
  value->words[2] = vector->w1;                                              \
  value->words[3] = vector->w0;                                              \
}                                                                            \
static test_u16 value_ok_##NAME(volatile test_f64 QUALIFIER *value,          \
                                const double_vector *vector)                 \
{                                                                            \
  return value->words[0] == vector->w3 && value->words[1] == vector->w2 &&   \
         value->words[2] == vector->w1 && value->words[3] == vector->w0;     \
}

DEFINE_VALUE_ACCESS(near, _near)
DEFINE_VALUE_ACCESS(far, _far)
DEFINE_VALUE_ACCESS(huge, _huge)

#define DEFINE_PACKED_ACCESS(NAME, QUALIFIER)                                \
static void set_packed_##NAME(                                               \
    _packed struct packed_double QUALIFIER *value)                           \
{                                                                            \
  test_u8 QUALIFIER *destination = (test_u8 QUALIFIER *)value;               \
  test_u8 _near *source = (test_u8 _near *)&near_source;                     \
  test_u16 index;                                                            \
  value->prefix = 0xa5U;                                                     \
  for (index = 0U; index < 8U; ++index)                                     \
    destination[index + 1U] = source[index];                                 \
}                                                                            \
static test_u16 packed_ok_##NAME(                                            \
    _packed struct packed_double QUALIFIER *value)                           \
{                                                                            \
  test_u8 QUALIFIER *actual = (test_u8 QUALIFIER *)value;                    \
  test_u8 _near *expected = (test_u8 _near *)&near_source;                   \
  test_u16 index;                                                            \
  if (value->prefix != 0xa5U)                                                \
    return 0U;                                                               \
  for (index = 0U; index < 8U; ++index)                                     \
    if (actual[index + 1U] != expected[index])                               \
      return 0U;                                                             \
  return 1U;                                                                 \
}

DEFINE_PACKED_ACCESS(near, _near)
DEFINE_PACKED_ACCESS(far, _far)
DEFINE_PACKED_ACCESS(huge, _huge)

static void copy_far(volatile double _far *destination,
                     volatile double _far *source)
{
  *destination = *source;
}

static void copy_huge(volatile double _huge *destination,
                      volatile double _huge *source)
{
  *destination = *source;
}

static void copy_far_to_near(volatile double _far *source)
{
  near_destination.value = *source;
}

static void copy_huge_to_near(volatile double _huge *source)
{
  near_destination.value = *source;
}

static void copy_packed_far_to_near(
    _packed struct packed_double _far *source)
{
  near_destination.value = source->value;
}

static void copy_packed_huge_to_near(
    _packed struct packed_double _huge *source)
{
  near_destination.value = source->value;
}

static void run_vector(test_u16 vector_index)
{
  const double_vector *vector = &vectors[vector_index];
  test_u16 base = (test_u16)(vector_index * 10U + 1U);

  set_value_near(&near_source, vector);
  set_value_far(&far_source, vector);
  set_value_huge(&huge_source, vector);

  near_destination.value = near_source.value;
  test_check(base, value_ok_near(&near_destination, vector));
  copy_far_to_near(&far_source.value);
  test_check((test_u16)(base + 1U),
             value_ok_near(&near_destination, vector));
  copy_huge_to_near(&huge_source.value);
  test_check((test_u16)(base + 2U),
             value_ok_near(&near_destination, vector));

  copy_far(&far_destination.value, &far_source.value);
  test_check((test_u16)(base + 3U),
             value_ok_far(&far_destination, vector));
  copy_huge(&huge_destination.value, &huge_source.value);
  test_check((test_u16)(base + 4U),
             value_ok_huge(&huge_destination, vector));

  set_packed_near(&packed_near_source);
  packed_near_destination.prefix = 0xa5U;
  packed_near_destination.value = packed_near_source.value;
  test_check((test_u16)(base + 5U),
             packed_ok_near(&packed_near_destination));

  set_packed_far(&packed_far_source);
  packed_far_destination.prefix = 0xa5U;
  packed_far_destination.value = packed_far_source.value;
  test_check((test_u16)(base + 6U),
             packed_ok_far(&packed_far_destination));

  set_packed_huge(&packed_huge_source);
  packed_huge_destination.prefix = 0xa5U;
  packed_huge_destination.value = packed_huge_source.value;
  test_check((test_u16)(base + 7U),
             packed_ok_huge(&packed_huge_destination));

  copy_packed_far_to_near(&packed_far_source);
  test_check((test_u16)(base + 8U),
             value_ok_near(&near_destination, vector));
  copy_packed_huge_to_near(&packed_huge_source);
  test_check((test_u16)(base + 9U),
             value_ok_near(&near_destination, vector));
}

void main(void)
{
  test_u16 vector_index;
  simulator_result = 0U;
  for (vector_index = 0U;
       vector_index < (test_u16)(sizeof(vectors) / sizeof(vectors[0]));
       ++vector_index)
    run_vector(vector_index);
  simulator_stop();
}
