# TinyJIT

TinyJIT is a lightweight Just-In-Time Compiler designed for the ARM64 architecture, specifically targeting the Mac Silicon ABI.

Currently it supports simple primitive types such as (float, int, long, char*)

* 32-bit and 64-bit Integers
* 32-bit floating-point numbers (single precision)
* Null-terminated strings (stored in static memory as arrays of characters)
* 4KB Code Memory
* 1MB Static Memory
* Branching with labels (Up to 16 labels supported for control flow)

It also has .data section for storing static memory (default 1MB) where strings are stored, you can store any type of buffer, LDR is also implemented.

# Example 

See main.c for more examples

```c

#define TINY_JIT_IMPLEMENTATION
#include "tiny_jit.h"

int main(void) {
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

  return 0;
}
```

# Result

```
./tiny_jit
3.140 + 2.718 = 5.858
```
