typedef signed int i16;
typedef unsigned int u16;
typedef signed long i32;
typedef unsigned long u32;

i32 convert_f32_i32(float value) { return (i32)value; }
u32 convert_f32_u32(float value) { return (u32)value; }
i16 convert_f32_i16(float value) { return (i16)value; }
u16 convert_f32_u16(float value) { return (u16)value; }
float convert_i32_f32(i32 value) { return (float)value; }
float convert_u32_f32(u32 value) { return (float)value; }
float convert_i16_f32(i16 value) { return (float)value; }
float convert_u16_f32(u16 value) { return (float)value; }
