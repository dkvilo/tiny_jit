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

# Dump
```
		TinyJIT Dump

INSTRUCTIONS: 8/4096 bytes
---------------------------------------------------------
00000000: 60 b8 9e d2 00 09 a8 f2

No data section found.
```

# Dump w/Data

```
		TinyJIT Dump

INSTRUCTIONS: 12/4096 bytes
---------------------------------------------------------
00000000: fd 7b 3f a9 20 00 00 90 fd 7b 3f a9

STATIC MEMORY: 104/1048576 bytes
---------------------------------------------------------
00000000: 48 65 6c 6c 6f 2c 20 54 68 65 72 65 21 20 54 68
00000010: 69 73 20 73 74 72 69 6e 67 20 69 73 20 73 74 6f
00000020: 72 65 64 20 69 6e 20 74 68 65 20 27 64 61 74 61
00000030: 27 20 73 65 63 74 69 6f 6e 2e 0a 00 00 00 00 00
00000040: 54 68 69 73 20 73 74 72 69 6e 67 20 69 73 20 61
00000050: 6c 73 6f 20 73 74 6f 72 65 64 20 74 68 65 72 65
00000060: 2e 0a 00 00 00 00 00 00
```
