typedef signed int i16;
typedef unsigned int u16;
typedef signed long i32;
typedef unsigned long u32;

i32 convert_f64_i32(double value) { return (i32)value; }
u32 convert_f64_u32(double value) { return (u32)value; }
i16 convert_f64_i16(double value) { return (i16)value; }
u16 convert_f64_u16(double value) { return (u16)value; }
double convert_i32_f64(i32 value) { return (double)value; }
double convert_u32_f64(u32 value) { return (double)value; }
double convert_i16_f64(i16 value) { return (double)value; }
double convert_u16_f64(u16 value) { return (double)value; }
