#include <math.h>
#include <stdio.h>

int
add_numbers(int a, int b)
{
  printf("Adding numbers in shared library: %d + %d\n", a, b);
  return a + b;
}

int
subtract_numbers(int a, int b)
{
  printf("Subtracting numbers in shared library: %d - %d\n", a, b);
  return a - b;
}

int
multiply_numbers(int a, int b)
{
  printf("Multiplying numbers in shared library: %d * %d\n", a, b);
  return a * b;
}

float
add_floats(float a, float b)
{
  printf("Adding floats in shared library: %f + %f\n", a, b);
  return a + b;
}

float
multiply_floats(float a, float b)
{
  printf("Multiplying floats in shared library: %f * %f\n", a, b);
  return a * b;
}

float
divide_floats(float a, float b)
{
  if (b == 0) {
    printf("Error: Division by zero!\n");
    return 0;
  }
  printf("Dividing floats in shared library: %f / %f\n", a, b);
  return a / b;
}

double
add_doubles(double a, double b)
{
  printf("Adding doubles in shared library: %lf + %lf\n", a, b);
  return a + b;
}

double
multiply_doubles(double a, double b)
{
  printf("Multiplying doubles in shared library: %lf * %lf\n", a, b);
  return a * b;
}

double
calculate_sqrt(double x)
{
  if (x < 0) {
    printf("Error: Cannot calculate square root of negative number!\n");
    return 0;
  }
  printf("Calculating square root of %lf\n", x);
  return sqrt(x);
}

double
calculate_power(double base, double exponent)
{
  printf("Calculating power: %lf ^ %lf\n", base, exponent);
  return pow(base, exponent);
}

float
calculate_hypotenuse(float a, float b)
{
  printf("Calculating hypotenuse with a=%f, b=%f\n", a, b);
  return sqrtf(a * a + b * b);
}

void
add_vectors(float* a, float* b, float* result, int size)
{
  printf("Adding vectors of size %d\n", size);
  for (int i = 0; i < size; i++) {
    result[i] = a[i] + b[i];
  }
}

void
multiply_matrices_2x2(float* a, float* b, float* result)
{
  printf("Multiplying 2x2 matrices\n");
  result[0] = a[0] * b[0] + a[1] * b[2];
  result[1] = a[0] * b[1] + a[1] * b[3];
  result[2] = a[2] * b[0] + a[3] * b[2];
  result[3] = a[2] * b[1] + a[3] * b[3];
}