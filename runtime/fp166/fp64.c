/* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * IEEE-754 binary64 arithmetic for the C166 runtime.
 *
 * Adapted from compiler-rt for the 16-bit TASKING C166 compiler.  This
 * implementation uses integer limbs only; see LICENSE.txt.
 */

typedef unsigned int u16;
typedef unsigned long u32;

typedef union {
  u32 all;
  struct {
    u16 low;
    u16 high;
  } word;
} u32_words;

typedef struct {
  u16 word[4];
} df_limbs;

enum {
  DF_FINITE,
  DF_ZERO,
  DF_INFINITY,
  DF_NAN
};

#define DF_SIGN          0x8000U
#define DF_FRACTION      0x000fU
#define DF_IMPLICIT      0x0010U
#define DF_INFINITY_BITS 0x7ff0U
#define DF_QUIET_BIT     0x0008U

static void df_store(u16 *destination, const df_limbs *value)
{
  destination[0] = value->word[3];
  destination[1] = value->word[2];
  destination[2] = value->word[1];
  destination[3] = value->word[0];
}

static void df_store_high(u16 *destination, u16 high)
{
  destination[0] = high;
  destination[1] = 0U;
  destination[2] = 0U;
  destination[3] = 0U;
}

static void df_copy(u16 *destination, const u16 *source)
{
  destination[0] = source[0];
  destination[1] = source[1];
  destination[2] = source[2];
  destination[3] = source[3];
}

static int df_unpack(df_limbs *value, const u16 *source, int *exponent,
                     u16 *sign)
{
  unsigned int input_exponent;
  u16 fraction;
  unsigned int shift;

  value->word[0] = source[3];
  value->word[1] = source[2];
  value->word[2] = source[1];
  value->word[3] = source[0] & DF_FRACTION;
  *sign = source[0] & DF_SIGN;
  input_exponent = (source[0] >> 4) & 0x07ffU;
  fraction = value->word[0] | value->word[1] |
             value->word[2] | value->word[3];

  if (input_exponent == 0x07ffU) {
    if (fraction == 0U)
      return DF_INFINITY;
    value->word[3] |= source[0] & DF_SIGN;
    value->word[3] |= DF_INFINITY_BITS | DF_QUIET_BIT;
    return DF_NAN;
  }
  if (input_exponent == 0U) {
    if (fraction == 0U) {
      value->word[3] = *sign;
      return DF_ZERO;
    }
    shift = 0U;
    while ((value->word[3] & DF_IMPLICIT) == 0U) {
      u16 carry0 = value->word[0] >> 15;
      u16 carry1 = value->word[1] >> 15;
      u16 carry2 = value->word[2] >> 15;
      value->word[0] <<= 1;
      value->word[1] = (value->word[1] << 1) | carry0;
      value->word[2] = (value->word[2] << 1) | carry1;
      value->word[3] = (value->word[3] << 1) | carry2;
      ++shift;
    }
    *exponent = 1 - (int)shift;
  } else {
    value->word[3] |= DF_IMPLICIT;
    *exponent = (int)input_exponent;
  }
  return DF_FINITE;
}

static int df_compare(const df_limbs *left, const df_limbs *right)
{
  unsigned int index = 4U;

  while (index-- != 0U) {
    if (left->word[index] != right->word[index])
      return left->word[index] > right->word[index] ? 1 : -1;
  }
  return 0;
}

static void df_shift_left_one(df_limbs *value)
{
  u16 carry = 0U;
  unsigned int index;

  for (index = 0U; index != 4U; ++index) {
    u16 current = value->word[index];
    value->word[index] = (current << 1) | carry;
    carry = current >> 15;
  }
}

static void df_shift_left_three(df_limbs *value)
{
  df_shift_left_one(value);
  df_shift_left_one(value);
  df_shift_left_one(value);
}

static void df_shift_right_sticky(df_limbs *value, unsigned int count)
{
  u16 sticky = 0U;

  if (count >= 64U) {
    unsigned int index;
    for (index = 0U; index != 4U; ++index) {
      sticky |= value->word[index];
      value->word[index] = 0U;
    }
  } else {
    while (count-- != 0U) {
      u16 lower = value->word[0];
      unsigned int index;
      sticky |= lower & 1U;
      for (index = 0U; index != 3U; ++index) {
        u16 higher = value->word[index + 1U];
        value->word[index] = (lower >> 1) | (higher << 15);
        lower = higher;
      }
      value->word[3] = lower >> 1;
    }
  }
  if (sticky != 0U)
    value->word[0] |= 1U;
}

static void df_shift_right_three(df_limbs *value)
{
  unsigned int count;

  for (count = 0U; count != 3U; ++count) {
    u16 lower = value->word[0];
    unsigned int index;
    for (index = 0U; index != 3U; ++index) {
      u16 higher = value->word[index + 1U];
      value->word[index] = (lower >> 1) | (higher << 15);
      lower = higher;
    }
    value->word[3] = lower >> 1;
  }
}

static void df_add_limbs(df_limbs *left, const df_limbs *right)
{
  u16 carry = 0U;
  unsigned int index;

  for (index = 0U; index != 4U; ++index) {
    u16 old = left->word[index];
    u16 sum = old + right->word[index];
    u16 next_carry = sum < old;
    old = sum;
    sum += carry;
    next_carry |= sum < old;
    left->word[index] = sum;
    carry = next_carry;
  }
}

static void df_sub_limbs(df_limbs *left, const df_limbs *right)
{
  u16 borrow = 0U;
  unsigned int index;

  for (index = 0U; index != 4U; ++index) {
    u16 old = left->word[index];
    u16 difference = old - right->word[index];
    u16 next_borrow = old < right->word[index];
    old = difference;
    difference -= borrow;
    next_borrow |= old < borrow;
    left->word[index] = difference;
    borrow = next_borrow;
  }
}

static void df_increment(df_limbs *value)
{
  unsigned int index;

  for (index = 0U; index != 4U; ++index) {
    if (++value->word[index] != 0U)
      break;
  }
}

static void df_round_pack(u16 *destination, df_limbs *value, int exponent,
                          u16 sign)
{
  unsigned int round_guard_sticky;

  if (exponent >= 0x07ff) {
    df_store_high(destination, sign | DF_INFINITY_BITS);
    return;
  }
  if (exponent <= 0) {
    df_shift_right_sticky(value, (unsigned int)(1 - exponent));
    exponent = 0;
  }

  round_guard_sticky = value->word[0] & 7U;
  df_shift_right_three(value);
  value->word[3] &= DF_FRACTION;
  value->word[3] |= (u16)(exponent << 4) | sign;
  if (round_guard_sticky > 4U ||
      (round_guard_sticky == 4U && (value->word[0] & 1U) != 0U))
    df_increment(value);
  df_store(destination, value);
}

static void df_addsub(u16 *destination, const u16 *left, const u16 *right,
                      u16 subtract)
{
  df_limbs a;
  df_limbs b;
  int a_exponent = 0;
  int b_exponent = 0;
  u16 a_sign;
  u16 b_input_sign;
  u16 b_sign;
  int a_kind;
  int b_kind;
  int magnitude_compare;
  unsigned int align;

  a_kind = df_unpack(&a, left, &a_exponent, &a_sign);
  b_kind = df_unpack(&b, right, &b_exponent, &b_input_sign);
  b_sign = b_input_sign ^ subtract;

  if (a_kind == DF_NAN) {
    df_store(destination, &a);
    return;
  }
  if (b_kind == DF_NAN) {
    df_store(destination, &b);
    return;
  }
  if (a_kind == DF_INFINITY) {
    if (b_kind == DF_INFINITY && a_sign != b_sign) {
      df_store_high(destination, 0x7ff8U);
      return;
    }
    df_store_high(destination, a_sign | DF_INFINITY_BITS);
    return;
  }
  if (b_kind == DF_INFINITY) {
    df_store_high(destination, b_sign | DF_INFINITY_BITS);
    return;
  }
  if (b_kind == DF_ZERO) {
    if (a_kind == DF_ZERO)
      df_store_high(destination, a_sign & b_sign);
    else
      df_copy(destination, left);
    return;
  }
  if (a_kind == DF_ZERO) {
    destination[0] = right[0] ^ subtract;
    destination[1] = right[1];
    destination[2] = right[2];
    destination[3] = right[3];
    return;
  }

  magnitude_compare = a_exponent == b_exponent ? df_compare(&a, &b) :
                      (a_exponent > b_exponent ? 1 : -1);
  if (magnitude_compare < 0) {
    df_limbs temporary;
    int temporary_exponent = a_exponent;
    u16 temporary_sign = a_sign;
    unsigned int index;
    for (index = 0U; index != 4U; ++index) {
      temporary.word[index] = a.word[index];
      a.word[index] = b.word[index];
      b.word[index] = temporary.word[index];
    }
    a_exponent = b_exponent;
    b_exponent = temporary_exponent;
    a_sign = b_sign;
    b_sign = temporary_sign;
  }

  df_shift_left_three(&a);
  df_shift_left_three(&b);
  align = (unsigned int)(a_exponent - b_exponent);
  if (align != 0U)
    df_shift_right_sticky(&b, align);

  if (a_sign != b_sign) {
    df_sub_limbs(&a, &b);
    if ((a.word[0] | a.word[1] | a.word[2] | a.word[3]) == 0U) {
      df_store_high(destination, 0U);
      return;
    }
    while ((a.word[3] & (DF_IMPLICIT << 3)) == 0U) {
      df_shift_left_one(&a);
      --a_exponent;
    }
  } else {
    df_add_limbs(&a, &b);
    if ((a.word[3] & (DF_IMPLICIT << 4)) != 0U) {
      df_shift_right_sticky(&a, 1U);
      ++a_exponent;
    }
  }
  df_round_pack(destination, &a, a_exponent, a_sign);
}

void dc166_df_add(u16 *destination, const u16 *left, const u16 *right)
{
  df_addsub(destination, left, right, 0U);
}

void dc166_df_sub(u16 *destination, const u16 *left, const u16 *right)
{
  df_addsub(destination, left, right, DF_SIGN);
}

static void df_multiply_limbs(u16 product[8], const df_limbs *left,
                              const df_limbs *right)
{
  unsigned int index;
  unsigned int left_index;

  for (index = 0U; index != 8U; ++index)
    product[index] = 0U;
  for (left_index = 0U; left_index != 4U; ++left_index) {
    u16 carry = 0U;
    unsigned int right_index;
    for (right_index = 0U; right_index != 4U; ++right_index) {
      u32_words partial;
      u16 old;
      unsigned int product_index = left_index + right_index;

      partial.all = (u32)left->word[left_index] *
                    (u32)right->word[right_index];
      old = partial.word.low;
      partial.word.low += product[product_index];
      partial.word.high += partial.word.low < old;
      old = partial.word.low;
      partial.word.low += carry;
      partial.word.high += partial.word.low < old;
      product[product_index] = partial.word.low;
      carry = partial.word.high;
    }
    product[left_index + 4U] = carry;
  }
}

void dc166_df_mul(u16 *destination, const u16 *left, const u16 *right)
{
  df_limbs a;
  df_limbs b;
  int a_exponent = 0;
  int b_exponent = 0;
  u16 a_sign;
  u16 b_sign;
  int a_kind;
  int b_kind;
  u16 result_sign;
  u16 product[8];
  df_limbs result;
  u16 high_product;
  int result_exponent;

  a_kind = df_unpack(&a, left, &a_exponent, &a_sign);
  b_kind = df_unpack(&b, right, &b_exponent, &b_sign);
  result_sign = (a_sign ^ b_sign) & DF_SIGN;

  if (a_kind == DF_NAN) {
    df_store(destination, &a);
    return;
  }
  if (b_kind == DF_NAN) {
    df_store(destination, &b);
    return;
  }
  if ((a_kind == DF_ZERO && b_kind == DF_INFINITY) ||
      (a_kind == DF_INFINITY && b_kind == DF_ZERO)) {
    df_store_high(destination, 0x7ff8U);
    return;
  }
  if (a_kind == DF_INFINITY || b_kind == DF_INFINITY) {
    df_store_high(destination, result_sign | DF_INFINITY_BITS);
    return;
  }
  if (a_kind == DF_ZERO || b_kind == DF_ZERO) {
    df_store_high(destination, result_sign);
    return;
  }

  df_multiply_limbs(product, &a, &b);
  result_exponent = a_exponent + b_exponent - 1023;
  high_product = (product[6] & 0x0200U) != 0U;
  result_exponent += high_product;
  if ((product[0] | product[1] | product[2]) != 0U)
    product[3] |= 1U;
  result.word[0] = product[3];
  result.word[1] = product[4];
  result.word[2] = product[5];
  result.word[3] = product[6];
  df_shift_right_sticky(&result, 1U + high_product);
  df_round_pack(destination, &result, result_exponent, result_sign);
}

static int df_compare_twice(const df_limbs *left, const df_limbs *right)
{
  u16 current = left->word[3];
  unsigned int index = 4U;

  while (index-- != 0U) {
    u16 next = index != 0U ? left->word[index - 1U] : 0U;
    u16 doubled = (current << 1) | (next >> 15);
    if (doubled != right->word[index])
      return doubled > right->word[index] ? 1 : -1;
    current = next;
  }
  return 0;
}

static void df_divide_shifted(df_limbs *quotient, df_limbs *numerator,
                              const df_limbs *denominator,
                              unsigned int shift)
{
  unsigned int index;

  quotient->word[0] = 0U;
  quotient->word[1] = 0U;
  quotient->word[2] = 0U;
  quotient->word[3] = 0U;
  for (index = 0U;; ++index) {
    if (df_compare(numerator, denominator) >= 0) {
      df_sub_limbs(numerator, denominator);
      quotient->word[0] |= 1U;
    }
    if (index == shift)
      break;
    df_shift_left_one(quotient);
    df_shift_left_one(numerator);
  }
}

void dc166_df_div(u16 *destination, const u16 *left, const u16 *right)
{
  df_limbs a;
  df_limbs b;
  int a_exponent = 0;
  int b_exponent = 0;
  u16 a_sign;
  u16 b_sign;
  int a_kind;
  int b_kind;
  u16 result_sign;
  int exponent_difference;
  int result_exponent;
  int numerator_comparison;
  u16 numerator_is_smaller;
  int subnormal_shift;
  unsigned int shift;
  df_limbs result;
  int rounding_comparison;

  a_kind = df_unpack(&a, left, &a_exponent, &a_sign);
  b_kind = df_unpack(&b, right, &b_exponent, &b_sign);
  result_sign = (a_sign ^ b_sign) & DF_SIGN;

  if (a_kind == DF_NAN) {
    df_store(destination, &a);
    return;
  }
  if (b_kind == DF_NAN) {
    df_store(destination, &b);
    return;
  }
  if (a_kind == b_kind && a_kind != DF_FINITE) {
    df_store_high(destination, 0x7ff8U);
    return;
  }
  if (a_kind == DF_INFINITY || b_kind == DF_ZERO) {
    df_store_high(destination, result_sign | DF_INFINITY_BITS);
    return;
  }
  if (b_kind == DF_INFINITY || a_kind == DF_ZERO) {
    df_store_high(destination, result_sign);
    return;
  }

  exponent_difference = a_exponent - b_exponent;
  result_exponent = exponent_difference + 1023;
  numerator_comparison = df_compare(&a, &b);
  numerator_is_smaller = numerator_comparison < 0;
  result_exponent -= numerator_is_smaller;
  if (result_exponent >= 0x07ff) {
    df_store_high(destination, result_sign | DF_INFINITY_BITS);
    return;
  }

  if (result_exponent > 0) {
    shift = 52U + numerator_is_smaller;
  } else {
    subnormal_shift = exponent_difference + 1074;
    if (subnormal_shift < 0) {
      df_store_high(destination, result_sign);
      if (subnormal_shift == -1 && numerator_comparison > 0)
        destination[3] = 1U;
      return;
    }
    shift = (unsigned int)subnormal_shift;
    result_exponent = 0;
  }

  df_divide_shifted(&result, &a, &b, shift);
  result.word[3] &= (u16)~DF_IMPLICIT;
  result.word[3] |= (u16)(result_exponent << 4) | result_sign;
  rounding_comparison = df_compare_twice(&a, &b);
  if (rounding_comparison > 0 ||
      (rounding_comparison == 0 && (result.word[0] & 1U) != 0U))
    df_increment(&result);
  df_store(destination, &result);
}

static u32 df_to_unsigned32(const u16 *source)
{
  df_limbs value;
  int exponent = 0;
  u16 sign;
  int kind;
  int unbiased;
  unsigned int shift;
  u32_words result;

  kind = df_unpack(&value, source, &exponent, &sign);
  if (kind != DF_FINITE)
    return 0UL;
  unbiased = exponent - 1023;
  if (unbiased < 0)
    return 0UL;
  if (unbiased > 31)
    return 0xffffffffUL;
  shift = (unsigned int)(52 - unbiased);
  while (shift-- != 0U) {
    u16 lower = value.word[0];
    unsigned int index;
    for (index = 0U; index != 3U; ++index) {
      u16 higher = value.word[index + 1U];
      value.word[index] = (lower >> 1) | (higher << 15);
      lower = higher;
    }
    value.word[3] = lower >> 1;
  }
  result.word.low = value.word[0];
  result.word.high = value.word[1];
  return result.all;
}

u32 dc166_df_to_u32(const u16 *source)
{
  if ((source[0] & DF_SIGN) != 0U)
    return 0UL;
  return df_to_unsigned32(source);
}

u32 dc166_df_to_i32(const u16 *source)
{
  u32 result = df_to_unsigned32(source);

  if ((source[0] & DF_SIGN) != 0U)
    result = 0UL - result;
  return result;
}

static unsigned int df_clz16(u16 value)
{
  unsigned int count = 0U;

  while ((value & 0x8000U) == 0U) {
    value <<= 1;
    ++count;
  }
  return count;
}

static void df_from_unsigned32(u16 *destination, u32 input, u16 sign)
{
  u32_words magnitude;
  u16 normalized_high;
  u16 normalized_low;
  unsigned int exponent;
  unsigned int shift;

  if (input == 0UL) {
    df_store_high(destination, 0U);
    return;
  }
  magnitude.all = input;
  if (magnitude.word.high != 0U) {
    shift = df_clz16(magnitude.word.high);
    exponent = 31U - shift;
    if (shift == 0U) {
      normalized_high = magnitude.word.high;
      normalized_low = magnitude.word.low;
    } else {
      normalized_high = (magnitude.word.high << shift) |
                        (magnitude.word.low >> (16U - shift));
      normalized_low = magnitude.word.low << shift;
    }
  } else {
    shift = df_clz16(magnitude.word.low);
    exponent = 15U - shift;
    normalized_high = magnitude.word.low << shift;
    normalized_low = 0U;
  }

  destination[0] = sign | (u16)((exponent + 1023U) << 4) |
                   ((normalized_high >> 11) & DF_FRACTION);
  destination[1] = (normalized_high << 5) | (normalized_low >> 11);
  destination[2] = normalized_low << 5;
  destination[3] = 0U;
}

void dc166_df_from_u32(u16 *destination, u32 value)
{
  df_from_unsigned32(destination, value, 0U);
}

void dc166_df_from_i32(u16 *destination, u32 value)
{
  u32_words input;
  u16 sign;

  input.all = value;
  sign = input.word.high & DF_SIGN;
  if (sign != 0U)
    value = 0UL - value;
  df_from_unsigned32(destination, value, sign);
}
