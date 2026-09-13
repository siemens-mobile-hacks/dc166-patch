/* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * IEEE-754 binary32 arithmetic for the C166 runtime.
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
} sf_value;

typedef struct {
  u16 low;
  u16 high;
} sf_limbs;

#define SF_SIGN          0x8000U
#define SF_FRACTION      0x007fU
#define SF_IMPLICIT      0x0080U
#define SF_INFINITY      0x7f80U
#define SF_QUIET_BIT     0x0040U
#define SF_CANONICAL_NAN 0x7fc00000UL

static sf_limbs sf_unpack(u32 value)
{
  sf_value input;
  sf_limbs result;

  input.all = value;
  result.low = input.word.low;
  result.high = input.word.high;
  return result;
}

static u32 sf_pack(sf_limbs value)
{
  sf_value result;

  result.word.low = value.low;
  result.word.high = value.high;
  return result.all;
}

static void sf_shift_left_one(sf_limbs *value)
{
  u16 carry = value->low >> 15;

  value->low <<= 1;
  value->high = (u16)(value->high << 1) | carry;
}

static void sf_shift_left_three(sf_limbs *value)
{
  value->high = (u16)(value->high << 3) | (u16)(value->low >> 13);
  value->low <<= 3;
}

static unsigned int sf_normalize(sf_limbs *value, u16 top_bit)
{
  unsigned int shift = 0U;

  while ((value->high & top_bit) == 0U) {
    sf_shift_left_one(value);
    ++shift;
  }
  return shift;
}

static void sf_shift_right_sticky(sf_limbs *value, unsigned int count)
{
  u16 sticky = 0U;

  if (count >= 32U) {
    sticky = (value->low | value->high) != 0U;
    value->low = 0U;
    value->high = 0U;
  } else {
    while (count-- != 0U) {
      sticky |= value->low & 1U;
      value->low = (value->low >> 1) | (value->high << 15);
      value->high >>= 1;
    }
  }
  value->low |= sticky;
}

static int sf_compare(const sf_limbs *left, const sf_limbs *right)
{
  if (left->high != right->high)
    return left->high > right->high ? 1 : -1;
  if (left->low != right->low)
    return left->low > right->low ? 1 : -1;
  return 0;
}

static void sf_add(sf_limbs *left, const sf_limbs *right)
{
  u16 old_low = left->low;

  left->low += right->low;
  left->high += right->high + (left->low < old_low);
}

static void sf_sub(sf_limbs *left, const sf_limbs *right)
{
  u16 old_low = left->low;

  left->low -= right->low;
  left->high -= right->high + (old_low < right->low);
}

static void sf_increment(sf_limbs *value)
{
  if (++value->low == 0U)
    ++value->high;
}

static u32 sf_round_pack(sf_limbs value, int exponent, u16 sign)
{
  unsigned int round_guard_sticky;
  u16 round_up;
  u16 old_low;

  if (exponent <= 0) {
    sf_shift_right_sticky(&value, (unsigned int)(1 - exponent));
    exponent = 0;
  }

  round_guard_sticky = value.low & 7U;
  value.low = (value.low >> 3) | (value.high << 13);
  value.high >>= 3;
  value.high &= SF_FRACTION;
  value.high |= (u16)(exponent << 7) | sign;

  round_up = (u16)(round_guard_sticky + (value.low & 1U) + 3U) >> 3;
  old_low = value.low;
  value.low += round_up;
  value.high += value.low < old_low;
  return sf_pack(value);
}

static u32 sf_addsub(u32 left, u32 right, u16 subtract)
{
  sf_limbs a = sf_unpack(left);
  sf_limbs b = sf_unpack(right);
  u16 a_sign = a.high & SF_SIGN;
  u16 b_input_sign = b.high & SF_SIGN;
  u16 b_sign = b_input_sign ^ subtract;
  int a_input_exponent;
  int b_input_exponent;
  u16 a_fraction;
  u16 b_fraction;
  u16 a_zero_bits;
  u16 b_zero_bits;
  int a_exponent;
  int b_exponent;
  unsigned int align;

  a.high &= ~SF_SIGN;
  b.high &= ~SF_SIGN;
  a_input_exponent = a.high >> 7;
  b_input_exponent = b.high >> 7;
  a_fraction = a.low | (u16)(a.high << 9);
  b_fraction = b.low | (u16)(b.high << 9);
  a_zero_bits = (u16)a_input_exponent | a_fraction;
  b_zero_bits = (u16)b_input_exponent | b_fraction;

  if (a_input_exponent == 0xff) {
    if (a_fraction != 0U) {
      a.high |= a_sign | SF_QUIET_BIT;
      return sf_pack(a);
    }
    if (b_input_exponent == 0xff) {
      if (b_fraction != 0U) {
        b.high |= b_input_sign | SF_QUIET_BIT;
        return sf_pack(b);
      }
      if (a_sign != b_sign)
        return SF_CANONICAL_NAN;
    }
    a.high |= a_sign;
    return sf_pack(a);
  }
  if (b_input_exponent == 0xff) {
    if (b_fraction != 0U) {
      b.high |= b_input_sign | SF_QUIET_BIT;
      return sf_pack(b);
    }
    b.high |= b_sign;
    return sf_pack(b);
  }

  if (b_zero_bits == 0U) {
    if (a_zero_bits == 0U) {
      b.high |= b_sign & a_sign;
      return sf_pack(b);
    }
    a.high |= a_sign;
    return sf_pack(a);
  }
  if (a_zero_bits == 0U) {
    b.high |= b_sign;
    return sf_pack(b);
  }

  if (sf_compare(&b, &a) > 0) {
    sf_limbs temporary = a;
    u16 temporary_sign = a_sign;

    a = b;
    b = temporary;
    a_sign = b_sign;
    b_sign = temporary_sign;
  }

  a_exponent = a.high >> 7;
  b_exponent = b.high >> 7;
  a.high &= SF_FRACTION;
  b.high &= SF_FRACTION;
  if (a_exponent == 0)
    a_exponent = 1 - (int)sf_normalize(&a, SF_IMPLICIT);
  if (b_exponent == 0)
    b_exponent = 1 - (int)sf_normalize(&b, SF_IMPLICIT);
  a.high |= SF_IMPLICIT;
  b.high |= SF_IMPLICIT;
  sf_shift_left_three(&a);
  sf_shift_left_three(&b);

  align = (unsigned int)(a_exponent - b_exponent);
  if (align != 0U)
    sf_shift_right_sticky(&b, align);

  if (a_sign != b_sign) {
    sf_sub(&a, &b);
    if ((a.low | a.high) == 0U)
      return 0UL;
    if ((a.high & (SF_IMPLICIT << 3)) == 0U)
      a_exponent -= (int)sf_normalize(&a, SF_IMPLICIT << 3);
  } else {
    sf_add(&a, &b);
    if ((a.high & (SF_IMPLICIT << 4)) != 0U) {
      sf_shift_right_sticky(&a, 1U);
      ++a_exponent;
    }
  }

  if (a_exponent >= 0xff) {
    a.low = 0U;
    a.high = a_sign | SF_INFINITY;
    return sf_pack(a);
  }
  if (a_exponent <= -31)
    return (u32)a_sign << 16;
  return sf_round_pack(a, a_exponent, a_sign);
}

static void sf_multiply(u16 product[3], const sf_limbs *left,
                        const sf_limbs *right)
{
  u16 a[2];
  u16 b[2];
  u16 words[4];
  unsigned int i;
  unsigned int j;

  a[0] = left->low;
  a[1] = left->high;
  b[0] = right->low;
  b[1] = right->high;
  words[0] = 0U;
  words[1] = 0U;
  words[2] = 0U;
  words[3] = 0U;

  for (i = 0U; i != 2U; ++i) {
    u16 carry = 0U;
    for (j = 0U; j != 2U; ++j) {
      sf_value partial;
      u16 old_low;

      partial.all = (u32)a[i] * (u32)b[j];
      old_low = partial.word.low;
      partial.word.low += words[i + j];
      partial.word.high += partial.word.low < old_low;
      old_low = partial.word.low;
      partial.word.low += carry;
      partial.word.high += partial.word.low < old_low;
      words[i + j] = partial.word.low;
      carry = partial.word.high;
    }
    words[i + 2U] = carry;
  }

  product[0] = words[0];
  product[1] = words[1];
  product[2] = words[2];
}

u32 dc166_sf_add(u32 left, u32 right)
{
  return sf_addsub(left, right, 0U);
}

u32 dc166_sf_sub(u32 left, u32 right)
{
  return sf_addsub(left, right, SF_SIGN);
}

u32 dc166_sf_mul(u32 left, u32 right)
{
  sf_limbs a = sf_unpack(left);
  sf_limbs b = sf_unpack(right);
  u16 result_sign = (a.high ^ b.high) & SF_SIGN;
  unsigned int a_exponent = (a.high >> 7) & 0xffU;
  unsigned int b_exponent = (b.high >> 7) & 0xffU;
  u16 a_fraction_zero = (a.low | (a.high & SF_FRACTION)) == 0U;
  u16 b_fraction_zero = (b.low | (b.high & SF_FRACTION)) == 0U;
  int a_effective_exponent;
  int b_effective_exponent;
  u16 product[3];
  u16 high_product;
  int result_exponent;
  sf_limbs result;
  unsigned int bit_shift;
  unsigned int inverse_shift;
  u16 discarded_mask;

  if (a_exponent == 0xffU) {
    if (!a_fraction_zero) {
      a.high |= SF_QUIET_BIT;
      return sf_pack(a);
    }
    if (b_exponent == 0xffU) {
      if (!b_fraction_zero) {
        b.high |= SF_QUIET_BIT;
        return sf_pack(b);
      }
    } else if (b_exponent == 0U && b_fraction_zero) {
      return SF_CANONICAL_NAN;
    }
    a.low = 0U;
    a.high = result_sign | SF_INFINITY;
    return sf_pack(a);
  }
  if (b_exponent == 0xffU) {
    if (!b_fraction_zero) {
      b.high |= SF_QUIET_BIT;
      return sf_pack(b);
    }
    if (a_exponent == 0U && a_fraction_zero)
      return SF_CANONICAL_NAN;
    b.low = 0U;
    b.high = result_sign | SF_INFINITY;
    return sf_pack(b);
  }

  if ((a_exponent == 0U && a_fraction_zero) ||
      (b_exponent == 0U && b_fraction_zero)) {
    a.low = 0U;
    a.high = result_sign;
    return sf_pack(a);
  }

  a.high &= SF_FRACTION;
  b.high &= SF_FRACTION;
  a_effective_exponent = (int)a_exponent;
  b_effective_exponent = (int)b_exponent;
  if (a_exponent == 0U)
    a_effective_exponent = 1 - (int)sf_normalize(&a, SF_IMPLICIT);
  if (b_exponent == 0U)
    b_effective_exponent = 1 - (int)sf_normalize(&b, SF_IMPLICIT);
  a.high |= SF_IMPLICIT;
  b.high |= SF_IMPLICIT;

  sf_multiply(product, &a, &b);
  result_exponent = a_effective_exponent + b_effective_exponent - 127;
  high_product = (product[2] & 0x8000U) != 0U;
  result_exponent += high_product;
  if (result_exponent >= 0xff) {
    result.low = 0U;
    result.high = result_sign | SF_INFINITY;
    return sf_pack(result);
  }
  if (result_exponent <= -31)
    return (u32)result_sign << 16;

  bit_shift = 4U + high_product;
  inverse_shift = 16U - bit_shift;
  result.low = (product[1] >> bit_shift) |
               (u16)(product[2] << inverse_shift);
  result.high = product[2] >> bit_shift;
  discarded_mask = ((u16)1U << bit_shift) - 1U;
  if ((product[0] | (product[1] & discarded_mask)) != 0U)
    result.low |= 1U;
  return sf_round_pack(result, result_exponent, result_sign);
}

static void sf_divide_shifted(sf_limbs *quotient, sf_limbs *numerator,
                              const sf_limbs *denominator,
                              unsigned int shift)
{
  unsigned int index;

  quotient->low = 0U;
  quotient->high = 0U;
  for (index = 0U;; ++index) {
    if (sf_compare(numerator, denominator) >= 0) {
      sf_sub(numerator, denominator);
      quotient->low |= 1U;
    }
    if (index == shift)
      break;
    sf_shift_left_one(quotient);
    sf_shift_left_one(numerator);
  }
}

u32 dc166_sf_div(u32 left, u32 right)
{
  sf_limbs a = sf_unpack(left);
  sf_limbs b = sf_unpack(right);
  u16 result_sign = (a.high ^ b.high) & SF_SIGN;
  unsigned int a_exponent = (a.high >> 7) & 0xffU;
  unsigned int b_exponent = (b.high >> 7) & 0xffU;
  u16 a_fraction = a.low | (a.high & SF_FRACTION);
  u16 b_fraction = b.low | (b.high & SF_FRACTION);
  u16 a_zero_bits = (u16)a_exponent | a_fraction;
  u16 b_zero_bits = (u16)b_exponent | b_fraction;
  int a_effective_exponent;
  int b_effective_exponent;
  int exponent_difference;
  int result_exponent;
  int comparison;
  int subnormal_shift;
  unsigned int shift;
  sf_limbs result;

  if (a_exponent == 0xffU) {
    if (a_fraction != 0U) {
      a.high |= SF_QUIET_BIT;
      return sf_pack(a);
    }
    if (b_exponent == 0xffU) {
      if (b_fraction != 0U) {
        b.high |= SF_QUIET_BIT;
        return sf_pack(b);
      }
      return SF_CANONICAL_NAN;
    }
    a.low = 0U;
    a.high = result_sign | SF_INFINITY;
    return sf_pack(a);
  }
  if (b_exponent == 0xffU) {
    if (b_fraction != 0U) {
      b.high |= SF_QUIET_BIT;
      return sf_pack(b);
    }
    b.low = 0U;
    b.high = result_sign;
    return sf_pack(b);
  }
  if (a_zero_bits == 0U) {
    if (b_zero_bits == 0U)
      return SF_CANONICAL_NAN;
    a.low = 0U;
    a.high = result_sign;
    return sf_pack(a);
  }
  if (b_zero_bits == 0U) {
    b.low = 0U;
    b.high = result_sign | SF_INFINITY;
    return sf_pack(b);
  }

  a.high &= SF_FRACTION;
  b.high &= SF_FRACTION;
  a_effective_exponent = (int)a_exponent;
  b_effective_exponent = (int)b_exponent;
  if (a_exponent == 0U)
    a_effective_exponent = 1 - (int)sf_normalize(&a, SF_IMPLICIT);
  if (b_exponent == 0U)
    b_effective_exponent = 1 - (int)sf_normalize(&b, SF_IMPLICIT);
  a.high |= SF_IMPLICIT;
  b.high |= SF_IMPLICIT;

  exponent_difference = a_effective_exponent - b_effective_exponent;
  result_exponent = exponent_difference + 127;
  comparison = sf_compare(&a, &b);
  if (comparison < 0)
    --result_exponent;
  if (result_exponent >= 0xff) {
    result.low = 0U;
    result.high = result_sign | SF_INFINITY;
    return sf_pack(result);
  }

  if (result_exponent > 0) {
    shift = (unsigned int)(exponent_difference + 150 - result_exponent);
  } else {
    subnormal_shift = exponent_difference + 149;
    if (subnormal_shift < 0) {
      result.low = 0U;
      result.high = result_sign;
      if (subnormal_shift == -1 && comparison > 0)
        result.low = 1U;
      return sf_pack(result);
    }
    shift = (unsigned int)subnormal_shift;
    result_exponent = 0;
  }

  sf_divide_shifted(&result, &a, &b, shift);
  result.high &= (u16)~SF_IMPLICIT;
  result.high |= (u16)(result_exponent << 7) | result_sign;
  sf_shift_left_one(&a);
  comparison = sf_compare(&a, &b);
  if (comparison > 0 || (comparison == 0 && (result.low & 1U) != 0U))
    sf_increment(&result);
  return sf_pack(result);
}

static unsigned int sf_clz16(u16 value)
{
  unsigned int count = 0U;

  while ((value & 0x8000U) == 0U) {
    value <<= 1;
    ++count;
  }
  return count;
}

static u32 sf_from_unsigned(u32 input, u16 sign)
{
  sf_value magnitude;
  sf_limbs result;
  u16 normalized_high;
  u16 normalized_low;
  unsigned int exponent;
  unsigned int shift;
  u16 round;
  u16 old_low;

  if (input == 0UL)
    return 0UL;
  magnitude.all = input;
  if (magnitude.word.high != 0U) {
    shift = sf_clz16(magnitude.word.high);
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
    shift = sf_clz16(magnitude.word.low);
    exponent = 15U - shift;
    normalized_high = magnitude.word.low << shift;
    normalized_low = 0U;
  }

  result.low = (normalized_low >> 8) | (normalized_high << 8);
  result.high = ((normalized_high >> 8) & SF_FRACTION) |
                (u16)((exponent + 127U) << 7) | sign;
  round = normalized_low & 0x00ffU;
  if (round > 0x0080U ||
      (round == 0x0080U && (result.low & 1U) != 0U)) {
    old_low = result.low;
    ++result.low;
    result.high += result.low < old_low;
  }
  return sf_pack(result);
}

u32 dc166_sf_from_u32(u32 value)
{
  return sf_from_unsigned(value, 0U);
}

u32 dc166_sf_from_i32(u32 value)
{
  sf_value input;
  u16 sign;

  input.all = value;
  sign = input.word.high & SF_SIGN;
  if (sign != 0U)
    value = 0UL - value;
  return sf_from_unsigned(value, sign);
}
