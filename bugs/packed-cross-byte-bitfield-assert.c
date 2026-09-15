typedef unsigned int u16;

_packed struct packed_bits {
  unsigned int first : 1;
  unsigned int second : 16;
};

u16 assign_packed_bits(u16 seed) {
  _packed struct packed_bits value;

  value.first = (seed >> 15) & 1U;
  value.second = seed ^ 0xbeefU;
  return value.second;
}
