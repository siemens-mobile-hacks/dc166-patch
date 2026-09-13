extern volatile float f32_a[4];
extern volatile float f32_b[4];
extern volatile double f64_a[4];
extern volatile double f64_b[4];

float evaluate_float(unsigned int operation, unsigned int index)
{
    volatile float local;

    switch (operation) {
    case 0: return f32_a[index];
    case 1: f32_b[index] = f32_a[index]; return f32_b[index];
    case 2: f32_b[index] += f32_a[index]; return f32_b[index];
    case 3: f32_b[index] *= f32_a[index]; return f32_b[index];
    case 4: local = f32_a[index]; local += f32_b[index]; return local;
    default: local = f32_a[index]; local *= f32_b[index]; return local;
    }
}

double evaluate_double(unsigned int operation, unsigned int index)
{
    volatile double local;

    switch (operation) {
    case 0: return f64_a[index];
    case 1: f64_b[index] = f64_a[index]; return f64_b[index];
    case 2: f64_b[index] += f64_a[index]; return f64_b[index];
    case 3: f64_b[index] *= f64_a[index]; return f64_b[index];
    case 4: local = f64_a[index]; local += f64_b[index]; return local;
    default: local = f64_a[index]; local *= f64_b[index]; return local;
    }
}
