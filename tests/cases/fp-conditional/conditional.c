float select_f32(float value, unsigned int select_value)
{
  return select_value == 0x5a5aU ? value : 12345.0f;
}

double select_f64(double value, unsigned int select_value)
{
  return select_value == 0x5a5aU ? value : 12345.0;
}
