#include "test.h"

#define COPY_WORDS 80U

typedef struct {
  test_u16 words[COPY_WORDS];
} word_block;

typedef _packed struct {
  test_u8 tag;
  test_u16 words[COPY_WORDS];
} byte_block;


static word_block _near word_near_source;
static word_block _near word_near_destination;
static word_block _far word_far_source;
static word_block _far word_far_destination;
static word_block _huge word_huge_source;
static word_block _huge word_huge_destination;

static byte_block _near byte_near_source;
static byte_block _near byte_near_destination;
static byte_block _far byte_far_source;
static byte_block _far byte_far_destination;
static byte_block _huge byte_huge_source;
static byte_block _huge byte_huge_destination;

#define DEFINE_WORD_COPY(NAME, DESTINATION_QUALIFIER, SOURCE_QUALIFIER)       \
static void NAME(word_block DESTINATION_QUALIFIER *destination,              \
                 word_block SOURCE_QUALIFIER *source)                        \
{                                                                            \
  *destination = *source;                                                    \
}

DEFINE_WORD_COPY(copy_word_nn, _near, _near)
DEFINE_WORD_COPY(copy_word_nf, _near, _far)
DEFINE_WORD_COPY(copy_word_nh, _near, _huge)
DEFINE_WORD_COPY(copy_word_fn, _far, _near)
DEFINE_WORD_COPY(copy_word_ff, _far, _far)
DEFINE_WORD_COPY(copy_word_fh, _far, _huge)
DEFINE_WORD_COPY(copy_word_hn, _huge, _near)
DEFINE_WORD_COPY(copy_word_hf, _huge, _far)
DEFINE_WORD_COPY(copy_word_hh, _huge, _huge)

#define DEFINE_BYTE_COPY(NAME, DESTINATION_QUALIFIER, SOURCE_QUALIFIER)       \
static void NAME(byte_block DESTINATION_QUALIFIER *destination,              \
                 byte_block SOURCE_QUALIFIER *source)                        \
{                                                                            \
  *destination = *source;                                                    \
}

DEFINE_BYTE_COPY(copy_byte_nn, _near, _near)
DEFINE_BYTE_COPY(copy_byte_nf, _near, _far)
DEFINE_BYTE_COPY(copy_byte_nh, _near, _huge)
DEFINE_BYTE_COPY(copy_byte_fn, _far, _near)
DEFINE_BYTE_COPY(copy_byte_ff, _far, _far)
DEFINE_BYTE_COPY(copy_byte_fh, _far, _huge)
DEFINE_BYTE_COPY(copy_byte_hn, _huge, _near)
DEFINE_BYTE_COPY(copy_byte_hf, _huge, _far)
DEFINE_BYTE_COPY(copy_byte_hh, _huge, _huge)

#define DEFINE_WORD_ACCESS(NAME, QUALIFIER)                                  \
static void fill_word_##NAME(word_block QUALIFIER *block, test_u16 seed)     \
{                                                                            \
  test_u16 index;                                                            \
  for (index = 0U; index < COPY_WORDS; ++index)                              \
    block->words[index] = (test_u16)(seed ^ (index * 257U));                  \
}                                                                            \
static test_u16 check_word_##NAME(word_block QUALIFIER *block, test_u16 seed)\
{                                                                            \
  test_u16 index;                                                            \
  for (index = 0U; index < COPY_WORDS; ++index)                              \
    if (block->words[index] != (test_u16)(seed ^ (index * 257U)))            \
      return 0U;                                                             \
  return 1U;                                                                 \
}

DEFINE_WORD_ACCESS(near, _near)
DEFINE_WORD_ACCESS(far, _far)
DEFINE_WORD_ACCESS(huge, _huge)

#define DEFINE_BYTE_ACCESS(NAME, QUALIFIER)                                  \
static void fill_byte_##NAME(byte_block QUALIFIER *block, test_u16 seed)     \
{                                                                            \
  test_u8 QUALIFIER *bytes = (test_u8 QUALIFIER *)block;                     \
  test_u16 index;                                                            \
  for (index = 0U; index < (test_u16)sizeof(*block); ++index)                \
    bytes[index] = (test_u8)(seed + index * 29U);                            \
}                                                                            \
static test_u16 check_byte_##NAME(byte_block QUALIFIER *block, test_u16 seed)\
{                                                                            \
  test_u8 QUALIFIER *bytes = (test_u8 QUALIFIER *)block;                     \
  test_u16 index;                                                            \
  for (index = 0U; index < (test_u16)sizeof(*block); ++index)                \
    if (bytes[index] != (test_u8)(seed + index * 29U))                       \
      return 0U;                                                             \
  return 1U;                                                                 \
}

DEFINE_BYTE_ACCESS(near, _near)
DEFINE_BYTE_ACCESS(far, _far)
DEFINE_BYTE_ACCESS(huge, _huge)

void main(void)
{
  simulator_result = 0U;

  fill_word_near(&word_near_source, 0x1101U);
  fill_word_far(&word_far_source, 0x2202U);
  fill_word_huge(&word_huge_source, 0x3303U);
  copy_word_nn(&word_near_destination, &word_near_source);
  test_check(1U, check_word_near(&word_near_destination, 0x1101U));
  copy_word_nf(&word_near_destination, &word_far_source);
  test_check(2U, check_word_near(&word_near_destination, 0x2202U));
  copy_word_nh(&word_near_destination, &word_huge_source);
  test_check(3U, check_word_near(&word_near_destination, 0x3303U));
  copy_word_fn(&word_far_destination, &word_near_source);
  test_check(4U, check_word_far(&word_far_destination, 0x1101U));
  copy_word_ff(&word_far_destination, &word_far_source);
  test_check(5U, check_word_far(&word_far_destination, 0x2202U));
  copy_word_fh(&word_far_destination, &word_huge_source);
  test_check(6U, check_word_far(&word_far_destination, 0x3303U));
  copy_word_hn(&word_huge_destination, &word_near_source);
  test_check(7U, check_word_huge(&word_huge_destination, 0x1101U));
  copy_word_hf(&word_huge_destination, &word_far_source);
  test_check(8U, check_word_huge(&word_huge_destination, 0x2202U));
  copy_word_hh(&word_huge_destination, &word_huge_source);
  test_check(9U, check_word_huge(&word_huge_destination, 0x3303U));

  fill_byte_near(&byte_near_source, 0x41U);
  fill_byte_far(&byte_far_source, 0x52U);
  fill_byte_huge(&byte_huge_source, 0x63U);
  copy_byte_nn(&byte_near_destination, &byte_near_source);
  test_check(10U, check_byte_near(&byte_near_destination, 0x41U));
  copy_byte_nf(&byte_near_destination, &byte_far_source);
  test_check(11U, check_byte_near(&byte_near_destination, 0x52U));
  copy_byte_nh(&byte_near_destination, &byte_huge_source);
  test_check(12U, check_byte_near(&byte_near_destination, 0x63U));
  copy_byte_fn(&byte_far_destination, &byte_near_source);
  test_check(13U, check_byte_far(&byte_far_destination, 0x41U));
  copy_byte_ff(&byte_far_destination, &byte_far_source);
  test_check(14U, check_byte_far(&byte_far_destination, 0x52U));
  copy_byte_fh(&byte_far_destination, &byte_huge_source);
  test_check(15U, check_byte_far(&byte_far_destination, 0x63U));
  copy_byte_hn(&byte_huge_destination, &byte_near_source);
  test_check(16U, check_byte_huge(&byte_huge_destination, 0x41U));
  copy_byte_hf(&byte_huge_destination, &byte_far_source);
  test_check(17U, check_byte_huge(&byte_huge_destination, 0x52U));
  copy_byte_hh(&byte_huge_destination, &byte_huge_source);
  test_check(18U, check_byte_huge(&byte_huge_destination, 0x63U));

  simulator_stop();
}
