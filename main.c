//
// $file tiny_jit.h
// $author David Kviloria <david@skystargames.com>
//
#define TINY_JIT_IMPLEMENTATION
#include "tiny_jit.h"

void
print_number(int n)
{
  printf("Number: %d\n", n);
}
void
print_string(const char* str)
{
  printf("%s\n", str);
}

void
string_example()
{
  JITCompiler* jit = jit_init();
  if (!jit) {
    fprintf(stderr, "Failed to initialize JIT compiler\n");
    return;
  }

  size_t str_offset = jit_add_string(
    jit, "Hello, There! This string is stored in the 'data' section.");
  if (str_offset == (size_t)-1) {
    fprintf(stderr, "Failed to add string\n");
    jit_cleanup(jit);
    return;
  }

  size_t str2_offset = jit_add_string(jit, "This string is also stored there.");
  if (str2_offset == (size_t)-1) {
    fprintf(stderr, "Failed to add string2\n");
    jit_cleanup(jit);
    return;
  }

  jit_emit(jit, arm64_stp(29, 30, 31, -2)); // begin frame

  jit_load_string_addr(jit, 0, str_offset);
  jit_call(jit, print_string);

  jit_load_string_addr(jit, 0, str2_offset);
  jit_call(jit, print_string);

  jit_emit(jit, arm64_ldp(29, 30, 31, 2)); // end frame
  jit_emit(jit, arm64_ret());

  jit_execute_int(jit);

  jit_cleanup(jit);
}

void
dynamic_lib_example()
{
  JITCompiler* jit = jit_init();
  if (!jit)
    return;

  ExternalLibrary* lib = ext_lib_init("./libmath.so");
  if (!lib) {
    fprintf(stderr, "Failed to load library\n");
    return;
  }

  int add_func_idx = ext_lib_load_function(lib, "add_numbers");
  int mul_func_idx = ext_lib_load_function(lib, "multiply_numbers");

  if (add_func_idx < 0 || mul_func_idx < 0) {
    ext_lib_cleanup(lib);
    return;
  }

  // @add_numbers(10, 20)
  jit_load_int(jit, 0, 10); // arg 1
  jit_load_int(jit, 1, 20); // arg 2
  jit_call_external(jit, lib->functions[add_func_idx]);

  jit_emit(jit, arm64_add(19, 0, 31)); // x19 = x0

  // @multiply_numbers(5, 6)
  jit_load_int(jit, 0, 5); // arg 1
  jit_load_int(jit, 1, 6); // arg 2
  jit_call_external(jit, lib->functions[mul_func_idx]);

  // here we add add_numbers result (from x19) to multiplication result and
  // return.
  jit_emit(jit, arm64_add(0, 0, 19));
  jit_emit(jit, arm64_ret());

  jit_end_frame(jit);

  int result = jit_execute_int(jit);
  printf("Result: (add + multiply): %d\n", result);

  ext_lib_cleanup(lib);
  jit_cleanup(jit);

  exit(0);
}

void
ldr_example()
{
  JITCompiler* jit = jit_init();
  if (!jit)
    return;

  float value = 3.14f;
  size_t data_offset = jit_add_string(jit, (const char*)&value);

  // load address of data into x0
  jit_load_string_addr(jit, 0, data_offset);

  // load float from memory into v0
  jit_emit(jit, arm64_ldrs(0, 0, 0)); // LDR s0, [x0]
  jit_emit(jit, arm64_ret());

  float result = jit_execute_float(jit);
  printf("Loaded value: %.3f\n", result);

  jit_cleanup(jit);
}

void
float_example()
{
  JITCompiler* jit = jit_init();
  if (!jit)
    return;

  float a = 3.14f;
  float b = 2.718f;

  jit_load_float(jit, 0, a);
  jit_load_float(jit, 1, b);
  jit_float_add(jit, 0, 0, 1);

  jit_emit(jit, arm64_ret());

  float result = jit_execute_float(jit);
  printf("%.3f + %.3f = %.3f\n", a, b, result);

  jit_cleanup(jit);
}

int
main()
{
  string_example();
  float_example();
  ldr_example();
  dynamic_lib_example();
  return 0;
}

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/> */