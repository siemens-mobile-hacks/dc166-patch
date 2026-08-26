#include "test.h"

#define COPY_WORDS 80U

typedef struct {
  test_u16 words[COPY_WORDS];
} word_block;

typedef _packed struct {
  test_u8 tag;
  test_u16 words[COPY_WORDS];
} byte_block;

static word_block word_source;
static word_block word_destination;
static byte_block byte_source;
static byte_block byte_destination;

static void copy_words(word_block *destination, word_block *source)
{
  *destination = *source;
}

static void copy_bytes(byte_block *destination, byte_block *source)
{
  *destination = *source;
}

void main(void)
{
  test_u16 index;
  test_u8 *source_bytes = (test_u8 *)&byte_source;
  test_u8 *destination_bytes = (test_u8 *)&byte_destination;
  simulator_result = 0U;

  for (index = 0U; index < COPY_WORDS; ++index)
    word_source.words[index] = (test_u16)(0x1101U ^ index * 257U);
  copy_words(&word_destination, &word_source);
  for (index = 0U; index < COPY_WORDS; ++index)
    test_check(1U, word_destination.words[index] ==
                   (test_u16)(0x1101U ^ index * 257U));

  for (index = 0U; index < (test_u16)sizeof(byte_source); ++index)
    source_bytes[index] = (test_u8)(0x41U + index * 29U);
  copy_bytes(&byte_destination, &byte_source);
  for (index = 0U; index < (test_u16)sizeof(byte_source); ++index)
    test_check(2U, destination_bytes[index] ==
                   (test_u8)(0x41U + index * 29U));
  simulator_stop();
}
