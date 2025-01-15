//
// $file tiny_jit.h
// $author David Kviloria <david@skystargames.com>
// @version 0.4
//
// This is a single header implementation approach so you need to do following:
//
//    #define TINY_JIT_IMPLEMENTATION
//    #include "tiny_jit.h"
//
#ifndef __TINY_JIT_H
#define __TINY_JIT_H

#include <dlfcn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// clang-format off
typedef enum Cond
{
  COND_EQ = 0x0, /* equal */
  COND_NE = 0x1, /* not equal */
  COND_CS = 0x2, /* carry set */
  COND_CC = 0x3, /* carry clear */
  COND_MI = 0x4, /* negative */
  COND_PL = 0x5, /* positive */
  COND_VS = 0x6, /* overflow */
  COND_VC = 0x7, /* no overflow */
  COND_HI = 0x8, /* unsigned higher */
  COND_LS = 0x9, /* unsigned lower */
  COND_GE = 0xa, /* signed >= */
  COND_LT = 0xb, /* signed < */
  COND_GT = 0xc, /* signed > */
  COND_LE = 0xd  /* signed <= */
} Cond;

typedef enum {
  // general purpose
  rx0 = 0, rx1, rx2, rx3, rx4, rx5, rx6,
  rx7, rx8, rx9, rx10, rx11, rx12,
  rx13, rx14, rx15, rx16, rx17,
  rx18, rx19, rx20, rx21, rx22,
  rx23, rx24, rx25, rx26, rx27, rx28,

  rx29, /* FP */ rx30, /* LR */ 
  rx31, /* ZR */ rx32 = 31, /* SP */

  // floating-point/SIMD registers
  rv0 = 0, rv1, rv2, rv3, rv4, rv5, rv6, rv7,
  rv8, rv9, rv10, rv11, rv12, rv13, rv14,
  rv15, rv16, rv17, rv18, rv19, rv20, rv21,
  rv22, rv23, rv24, rv25, rv26, rv27,
  rv28, rv29, rv30, rv31,
} JITRegister;
// clang-format on

#define MAX_CODE_MEMORY_SIZE 4096          // 4K
#define MAX_DATA_MEMORY_SIZE (1024 * 1024) // 1MB
#define MAX_LABEL_CAPACITY 16

typedef struct
{
  // code
  uint32_t* code;
  size_t code_size;
  size_t capacity;

  // labels
  uint32_t** label_positions;
  size_t* label_offsets;
  size_t num_labels;
  size_t label_capacity;

  // data
  uint8_t* data;
  size_t data_size;
  size_t data_capacity;
} JITCompiler;

uint32_t
arm64_mov(int rd, int rs);

uint32_t
arm64_movz(int reg, uint16_t value);

uint32_t
arm64_movk(int reg, uint16_t value, int shift);

uint32_t
arm64_adr(int rd, int32_t offset);

uint32_t
arm64_add(int rd, int rn, int rm);

uint32_t
arm64_sub(int rd, int rn, int rm);

uint32_t
arm64_mul(int rd, int rn, int rm);

uint32_t
arm64_cmp(int rn, int rm);

uint32_t
arm64_b(int32_t offset);

uint32_t
arm64_b_cond(int32_t offset, int cond);

uint32_t
arm64_fmov_s(int rd, float value);

uint32_t
arm64_fadd_s(int rd, int rn, int rm);

uint32_t
arm64_fsub_s(int rd, int rn, int rm);

uint32_t
arm64_fmul_s(int rd, int rn, int rm);

uint32_t
arm64_fdiv_s(int rd, int rn, int rm);

uint32_t
arm64_fcmp_s(int rn, int rm);

uint32_t
arm64_scvtf_s(int rd, int rn);

uint32_t
arm64_fcvtzs_s(int rd, int rn);

void
jit_load_float(JITCompiler* jit, int reg, float value);
void
jit_float_add(JITCompiler* jit, int rd, int rn, int rm);
void
jit_float_sub(JITCompiler* jit, int rd, int rn, int rm);
void
jit_float_mul(JITCompiler* jit, int rd, int rn, int rm);
void
jit_float_div(JITCompiler* jit, int rd, int rn, int rm);
void
jit_float_compare(JITCompiler* jit, int rn, int rm);
void
jit_int_to_float(JITCompiler* jit, int rd, int rn);
void
jit_float_to_int(JITCompiler* jit, int rd, int rn);

uint32_t
arm64_bl(int32_t offset);

uint32_t
arm64_ret();

uint32_t
arm64_stp(int rt1, int rt2, int rn, int imm);

uint32_t
arm64_ldp(int rt1, int rt2, int rn, int imm);

uint32_t
arm64_adrp(int rd, int64_t imm);

uint32_t
arm64_add_imm(int rd, int rn, uint16_t imm12);

JITCompiler*
jit_init();

void
jit_dump_code(JITCompiler* jit);

size_t
jit_add_string(JITCompiler* jit, const char* str);

void
jit_load_string_addr(JITCompiler* jit, int reg, size_t offset);

const char*
jit_get_string(JITCompiler* jit, size_t offset);

void
jit_cleanup(JITCompiler* jit);

void
jit_reset(JITCompiler* jit);

void
jit_emit(JITCompiler* jit, uint32_t instruction);

size_t
jit_create_label(JITCompiler* jit);

void
jit_bind_label(JITCompiler* jit, size_t label);

int32_t
jit_branch_offset(JITCompiler* jit, size_t label);

void
jit_load_int(JITCompiler* jit, int reg, int32_t value);

void
jit_compare(JITCompiler* jit, int reg1, int reg2);

void
jit_jump(JITCompiler* jit, size_t label);

void
jit_jump_if_equal(JITCompiler* jit, size_t label);

void
jit_jump_if_not_equal(JITCompiler* jit, size_t label);

void
jit_jump_if_less(JITCompiler* jit, size_t label);

void
jit_jump_if_greater(JITCompiler* jit, size_t label);

void
jit_call(JITCompiler* jit, void* func_ptr);

typedef int (*JitFunctionInt)();
typedef float (*JitFunctionFloat)();
typedef double (*JitFunctionDouble)();

typedef union
{
  int i;
  float f;
  double d;
} JitValue;

typedef enum
{
  JIT_TYPE_INT,
  JIT_TYPE_FLOAT,
  JIT_TYPE_DOUBLE
} JitReturnType;

JitValue
jit_execute_typed(JITCompiler* jit, JitReturnType return_type);

int
jit_execute_int(JITCompiler* jit);

float
jit_execute_float(JITCompiler* jit);

double
jit_execute_double(JITCompiler* jit);

typedef struct
{
  void* handle;        // from dlopen
  void* functions[32]; // array of function pointers
  int func_count;
} ExternalLibrary;

ExternalLibrary*
ext_lib_init(const char* library_path);

int
ext_lib_load_function(ExternalLibrary* lib, const char* func_name);

void
ext_lib_cleanup(ExternalLibrary* lib);

void
jit_begin_frame(JITCompiler* jit);

void
jit_end_frame(JITCompiler* jit);

void
jit_call_external(JITCompiler* jit, void* func_ptr);

#if defined(TINY_JIT_IMPLEMENTATION)
uint32_t
arm64_mov(int rd, int rs)
{
  return 0xAA0003E0 | (rs << 16) | rd;
}

uint32_t
arm64_movz(int reg, uint16_t value)
{
  return 0xd2800000 | ((uint32_t)value << 5) | reg;
}

uint32_t
arm64_movk(int reg, uint16_t value, int shift)
{
  return 0xf2800000 | ((uint32_t)value << 5) | (shift << 21) | reg;
}

uint32_t
arm64_adr(int rd, int32_t offset)
{
  return 0x10000000 | ((offset & 0x7ffff) << 5) | rd;
}

uint32_t
arm64_add(int rd, int rn, int rm)
{
  return 0x8b000000 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_sub(int rd, int rn, int rm)
{
  return 0xcb000000 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_mul(int rd, int rn, int rm)
{
  return 0x9b007c00 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_cmp(int rn, int rm)
{
  return 0xeb000000 | (rm << 16) | (rn << 5);
}

uint32_t
arm64_b(int32_t offset)
{
  return 0x14000000 | (offset & 0x3ffffff);
}

uint32_t
arm64_b_cond(int32_t offset, int cond)
{
  uint32_t imm19 = offset & 0x7ffff;
  return 0x54000000 | (imm19 << 5) | cond;
}

void
jit_load_float(JITCompiler* jit, int reg, float value)
{
  union
  {
    float f;
    uint32_t i;
  } conv = { .f = value };

  uint16_t lower = conv.i & 0xFFFF;
  uint16_t upper = (conv.i >> 16) & 0xFFFF;

  // load into x0 using MOVZ/MOVK
  jit_emit(jit, 0xD2800000 | (lower << 5)); // MOVZ x0, #lower16
  jit_emit(jit, 0xF2A00000 | (upper << 5)); // MOVK x0, #upper16, LSL #16

  jit_emit(jit, 0x9E670000 | reg); // FMOV sN, w0
}

uint32_t
arm64_fadd_s(int rd, int rn, int rm)
{
  return 0x1E202800 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_fsub_s(int rd, int rn, int rm)
{
  return 0x1E203800 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_fmul_s(int rd, int rn, int rm)
{
  return 0x1E200800 | (rm << 16) | (rn << 5) | rd;
}

uint32_t
arm64_fdiv_s(int rd, int rn, int rm)
{
  return 0x1E201800 | (rm << 16) | (rn << 5) | rd;
}

// LDR (immediate) with 12-bit unsigned immediate offset
uint32_t
arm64_ldr(int rt, int rn, uint16_t imm12)
{
  return 0xF8400000 | ((imm12 & 0xFFF) << 10) | (rn << 5) | rt;
}

// LDR (immediate) 32-bit word
uint32_t
arm64_ldrw(int rt, int rn, uint16_t imm12)
{
  return 0xB8400000 | ((imm12 & 0xFFF) << 10) | (rn << 5) | rt;
}

// LDR (floating point) single precision
uint32_t
arm64_ldrs(int rt, int rn, uint16_t imm12)
{
  return 0xBC400000 | ((imm12 & 0xFFF) << 10) | (rn << 5) | rt;
}

// LDR with register offset
uint32_t
arm64_ldr_reg(int rt, int rn, int rm, int shift)
{
  return 0xF8600000 | (rm << 16) | ((shift & 0x7) << 12) | (rn << 5) | rt;
}

uint32_t
arm64_fcmp_s(int rn, int rm)
{
  return 0x1E202000 | (rm << 16) | (rn << 5);
}

// converts signed integer to single precision float
uint32_t
arm64_scvtf_s(int rd, int rn)
{
  return 0x1E220000 | (rn << 5) | rd;
}

// converts single precision float to signed integer
uint32_t
arm64_fcvtzs_s(int rd, int rn)
{
  return 0x1E380000 | (rn << 5) | rd;
}

void
jit_load_mem(JITCompiler* jit, int rt, int rn, uint16_t offset)
{
  jit_emit(jit, arm64_ldr(rt, rn, offset));
}

void
jit_load_mem_word(JITCompiler* jit, int rt, int rn, uint16_t offset)
{
  jit_emit(jit, arm64_ldrw(rt, rn, offset));
}

void
jit_load_float_mem(JITCompiler* jit, int rt, int rn, uint16_t offset)
{
  jit_emit(jit, arm64_ldrs(rt, rn, offset));
}

void
jit_float_add(JITCompiler* jit, int rd, int rn, int rm)
{
  jit_emit(jit, arm64_fadd_s(rd, rn, rm));
}

void
jit_float_sub(JITCompiler* jit, int rd, int rn, int rm)
{
  jit_emit(jit, arm64_fsub_s(rd, rn, rm));
}

void
jit_float_mul(JITCompiler* jit, int rd, int rn, int rm)
{
  jit_emit(jit, arm64_fmul_s(rd, rn, rm));
}

void
jit_float_div(JITCompiler* jit, int rd, int rn, int rm)
{
  jit_emit(jit, arm64_fdiv_s(rd, rn, rm));
}

void
jit_float_compare(JITCompiler* jit, int rn, int rm)
{
  jit_emit(jit, arm64_fcmp_s(rn, rm));
}

void
jit_int_to_float(JITCompiler* jit, int rd, int rn)
{
  jit_emit(jit, arm64_scvtf_s(rd, rn));
}

void
jit_float_to_int(JITCompiler* jit, int rd, int rn)
{
  jit_emit(jit, arm64_fcvtzs_s(rd, rn));
}

uint32_t
arm64_bl(int32_t offset)
{
  return 0x94000000 | ((offset & 0x3ffffff) << 0);
}

uint32_t
arm64_ret()
{
  return 0xd65f03c0;
}

uint32_t
arm64_stp(int rt1, int rt2, int rn, int imm)
{
  return 0xa9000000 | ((imm & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

uint32_t
arm64_ldp(int rt1, int rt2, int rn, int imm)
{
  return 0xa9400000 | ((imm & 0x7f) << 15) | (rt2 << 10) | (rn << 5) | rt1;
}

uint32_t
arm64_adrp(int rd, int64_t imm)
{
  uint32_t immlo = (imm >> 12) & 0x3;
  uint32_t immhi = (imm >> 14) & 0x7FFFF;
  return 0x90000000 | (immlo << 29) | (immhi << 5) | rd;
}

uint32_t
arm64_add_imm(int rd, int rn, uint16_t imm12)
{
  return 0x91000000 | ((uint32_t)imm12 << 10) | (rn << 5) | rd;
}

JITCompiler*
jit_init()
{
  JITCompiler* jit = malloc(sizeof(JITCompiler));
  if (!jit)
    return NULL;

  jit->capacity = MAX_CODE_MEMORY_SIZE;

#ifdef __APPLE__
  jit->code = mmap(NULL,
                   jit->capacity,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_JIT,
                   -1,
                   0);
#else
  jit->code = mmap(NULL,
                   jit->capacity,
                   PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1,
                   0);
#endif

  if (jit->code == MAP_FAILED) {
    free(jit);
    return NULL;
  }
  jit->code_size = 0;

  jit->data_capacity = MAX_DATA_MEMORY_SIZE;
  jit->data = mmap(NULL,
                   jit->data_capacity,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1,
                   0);

  if (jit->data == MAP_FAILED) {
    munmap(jit->code, jit->capacity);
    free(jit);
    return NULL;
  }
  jit->data_size = 0;

  jit->label_capacity = MAX_LABEL_CAPACITY;
  jit->label_positions = malloc(sizeof(uint32_t*) * jit->label_capacity);
  jit->label_offsets = malloc(sizeof(size_t) * jit->label_capacity);
  jit->num_labels = 0;

  if (!jit->label_positions || !jit->label_offsets) {
    if (jit->label_positions)
      free(jit->label_positions);
    if (jit->label_offsets)
      free(jit->label_offsets);
    munmap(jit->data, jit->data_capacity);
    munmap(jit->code, jit->capacity);
    free(jit);
    return NULL;
  }

  return jit;
}

size_t
jit_add_string(JITCompiler* jit, const char* str)
{
  size_t len = strlen(str) + 1;
  size_t aligned_size = (len + 7) & ~7; // 8-byte alignment

  if (jit->data_size + aligned_size > jit->data_capacity) {
    size_t new_capacity = jit->data_capacity * 2;
    void* new_data = mmap(NULL,
                          new_capacity,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1,
                          0);

    if (new_data == MAP_FAILED) {
      return (size_t)-1;
    }

    memcpy(new_data, jit->data, jit->data_size);
    munmap(jit->data, jit->data_capacity);

    jit->data = new_data;
    jit->data_capacity = new_capacity;
  }

  size_t offset = jit->data_size;
  memcpy(jit->data + offset, str, len);
  memset(jit->data + offset + len, 0, aligned_size - len); // zeoro padding
  jit->data_size += aligned_size;

  return offset;
}

void
jit_load_string_addr(JITCompiler* jit, int reg, size_t offset)
{
  uint64_t addr = (uint64_t)(jit->data + offset);

  // address and offset within page
  uint64_t page = addr & ~0xFFF;
  uint64_t page_offset = addr & 0xFFF;

  // relative page offset from PC
  int64_t pc = (int64_t)&jit->code[jit->code_size];
  int64_t rel_page = page - (pc & ~0xFFF);

  jit_emit(jit, arm64_adrp(reg, rel_page));
  if (page_offset) {
    jit_emit(jit, arm64_add_imm(reg, reg, page_offset));
  }
}

const char*
jit_get_string(JITCompiler* jit, size_t offset)
{
  if (offset >= jit->data_size)
    return NULL;
  return (const char*)(jit->data + offset);
}

void
jit_cleanup(JITCompiler* jit)
{
  if (!jit)
    return;
  if (jit->code)
    munmap(jit->code, jit->capacity);
  if (jit->data)
    munmap(jit->data, jit->data_capacity);
  if (jit->label_positions)
    free(jit->label_positions);
  if (jit->label_offsets)
    free(jit->label_offsets);
  free(jit);
}

void
jit_reset(JITCompiler* jit)
{
  if (!jit)
    return;
  memset(jit->code, 0, jit->capacity);
  jit->code_size = 0;
  memset(jit->label_positions, 0, jit->label_capacity * sizeof(uint32_t*));
  memset(jit->label_offsets, 0, jit->label_capacity * sizeof(size_t));
  jit->num_labels = 0;
}

void
jit_emit(JITCompiler* jit, uint32_t instruction)
{
  if (!jit || !jit->code) {
    fprintf(stderr, "Invalid JIT compiler state\n");
    return;
  }

  // resize if we need to grow the buffer
  if (jit->code_size >= (jit->capacity / sizeof(uint32_t)) - 16) {
    size_t new_capacity = jit->capacity * 2;
    uint32_t* new_code = mmap(NULL,
                              new_capacity,
                              PROT_READ | PROT_WRITE | PROT_EXEC,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1,
                              0);

    if (new_code == MAP_FAILED) {
      fprintf(stderr, "Failed to allocate new code buffer\n");
      return;
    }

    memcpy(new_code, jit->code, jit->code_size * sizeof(uint32_t));
    if (munmap(jit->code, jit->capacity) == -1) {
      fprintf(stderr, "Failed to unmap old code buffer\n");
      munmap(new_code, new_capacity);
      return;
    }

    printf(
      "JIT buffer grown from %zu to %zu bytes\n", jit->capacity, new_capacity);

    jit->code = new_code;
    jit->capacity = new_capacity;
  }

  jit->code[jit->code_size++] = instruction;
}

size_t
jit_create_label(JITCompiler* jit)
{
  if (jit->num_labels >= jit->label_capacity) {
    jit->label_capacity *= 2;
    jit->label_positions =
      realloc(jit->label_positions, sizeof(uint32_t*) * jit->label_capacity);
    jit->label_offsets =
      realloc(jit->label_offsets, sizeof(size_t) * jit->label_capacity);
  }

  jit->label_positions[jit->num_labels] = NULL;
  return jit->num_labels++;
}

void
jit_bind_label(JITCompiler* jit, size_t label)
{
  if (label >= jit->num_labels)
    return;
  jit->label_positions[label] = &jit->code[jit->code_size];
  jit->label_offsets[label] = jit->code_size;
}

int32_t
jit_branch_offset(JITCompiler* jit, size_t label)
{
  if (label >= jit->num_labels || !jit->label_positions[label])
    return 0;
  int32_t current = jit->code_size;
  int32_t target = jit->label_offsets[label];
  return (target - (current + 1));
}

void
jit_load_int(JITCompiler* jit, int reg, int32_t value)
{
  jit_emit(jit, arm64_movz(reg, value & 0xffff));
  if (value > 0xffff) {
    jit_emit(jit, arm64_movk(reg, (value >> 16) & 0xffff, 16));
  }
}

void
jit_compare(JITCompiler* jit, int reg1, int reg2)
{
  jit_emit(jit, arm64_cmp(reg1, reg2));
}

void
jit_jump(JITCompiler* jit, size_t label)
{
  int32_t offset = jit_branch_offset(jit, label);
  jit_emit(jit, arm64_b(offset));
}

void
jit_jump_if_equal(JITCompiler* jit, size_t label)
{
  int32_t offset = jit_branch_offset(jit, label);
  jit_emit(jit, arm64_b_cond(offset, COND_EQ));
}

void
jit_jump_if_not_equal(JITCompiler* jit, size_t label)
{
  int32_t offset = jit_branch_offset(jit, label);
  jit_emit(jit, arm64_b_cond(offset, COND_NE));
}

void
jit_jump_if_less(JITCompiler* jit, size_t label)
{
  int32_t offset = jit_branch_offset(jit, label);
  jit_emit(jit, arm64_b_cond(offset, COND_LT));
}

void
jit_jump_if_greater(JITCompiler* jit, size_t label)
{
  int32_t offset = jit_branch_offset(jit, label);
  jit_emit(jit, arm64_b_cond(offset, COND_GT));
}

void
jit_call(JITCompiler* jit, void* func_ptr)
{
  jit_emit(jit, arm64_stp(29, 30, 31, -2)); // stp x29, x30, [sp, #-16]!

  // note (david) probably we need tocheck if offset is valid.
  int64_t offset = (int64_t)func_ptr - (int64_t)&jit->code[jit->code_size];

  jit_emit(jit, arm64_bl(offset / 4));     // bl <offset>
  jit_emit(jit, arm64_ldp(29, 30, 31, 2)); // ldp x29, x30, [sp], #16
}

JitValue
jit_execute_typed(JITCompiler* jit, JitReturnType return_type)
{
  JitValue result = { 0 };

#ifdef __APPLE__
  if (mprotect(jit->code, jit->capacity, PROT_READ | PROT_EXEC) == -1) {
    perror("mprotect failed");
    return result;
  }
#endif

  switch (return_type) {
    case JIT_TYPE_INT: {
      JitFunctionInt func = (JitFunctionInt)jit->code;
      result.i = func();
      break;
    }
    case JIT_TYPE_FLOAT: {
      JitFunctionFloat func = (JitFunctionFloat)jit->code;
      result.f = func();
      break;
    }
    case JIT_TYPE_DOUBLE: {
      JitFunctionDouble func = (JitFunctionDouble)jit->code;
      result.d = func();
      break;
    }
  }

#ifdef __APPLE__
  if (mprotect(jit->code, jit->capacity, PROT_READ | PROT_WRITE) == -1) {
    perror("mprotect failed");
    return result;
  }
#endif

  return result;
}

void
jit_dump_code(JITCompiler* jit)
{
  if (!jit || !jit->code || jit->code_size == 0) {
    fprintf(stderr,
            "JIT dump failed: Invalid JIT compiler or empty code section.\n");
    return;
  }
  printf("\n\n\t\tTinyJIT Dump\n\n");
  printf("INSTRUCTIONS: %zu/%zu bytes\n", jit->code_size, jit->capacity);
  printf("---------------------------------------------------------\n");
  uint8_t* code = (uint8_t*)jit->code;
  for (size_t i = 0; i < jit->code_size; i++) {
    if (i % 16 == 0) {
      if (i > 0)
        printf("\n");
      printf("%08zx: ", i);
    }
    printf("%02x ", code[i]);
  }
  printf("\n");

  if (jit->data && jit->data_size > 0) {
    printf("\nSTATIC MEMORY: %zu/%zu bytes\n", jit->data_size, jit->data_capacity);
    printf("---------------------------------------------------------\n");
    uint8_t* data = (uint8_t*)jit->data;
    for (size_t i = 0; i < jit->data_size; i++) {
      if (i % 16 == 0) {
        if (i > 0)
          printf("\n");
        printf("%08zx: ", i);
      }
      printf("%02x ", data[i]);
    }
    printf("\n");
  } else {
    printf("\nNo data section found.\n");
  }
}

int
jit_execute_int(JITCompiler* jit)
{
  return jit_execute_typed(jit, JIT_TYPE_INT).i;
}

float
jit_execute_float(JITCompiler* jit)
{
  return jit_execute_typed(jit, JIT_TYPE_FLOAT).f;
}

double
jit_execute_double(JITCompiler* jit)
{
  return jit_execute_typed(jit, JIT_TYPE_DOUBLE).d;
}

ExternalLibrary*
ext_lib_init(const char* library_path)
{
  ExternalLibrary* lib = malloc(sizeof(ExternalLibrary));
  lib->handle = dlopen(library_path, RTLD_LAZY);
  if (!lib->handle) {
    fprintf(stderr, "Error loading library: %s\n", dlerror());
    free(lib);
    return NULL;
  }
  lib->func_count = 0;
  return lib;
}

int
ext_lib_load_function(ExternalLibrary* lib, const char* func_name)
{
  if (lib->func_count >= 32) {
    fprintf(stderr, "Too many functions loaded\n");
    return -1;
  }

  void* func = dlsym(lib->handle, func_name);
  if (!func) {
    fprintf(stderr, "Error loading function %s: %s\n", func_name, dlerror());
    return -1;
  }

  lib->functions[lib->func_count] = func;
  return lib->func_count++;
}

void
ext_lib_cleanup(ExternalLibrary* lib)
{
  if (lib) {
    if (lib->handle) {
      dlclose(lib->handle);
    }
    free(lib);
  }
}

void
jit_begin_frame(JITCompiler* jit)
{
  // save the frame pointer and link register
  jit_emit(jit, arm64_stp(29, 30, 31, -2)); // stp x29, x30, [sp, #-16]!
  jit_emit(jit, 0x910003fd);                // mov x29, sp
}

void
jit_end_frame(JITCompiler* jit)
{
  // restore frame pointer and link register
  jit_emit(jit, arm64_ldp(29, 30, 31, 2)); // ldp x29, x30, [sp], #16
  jit_emit(jit, arm64_ret());              // ret
}

void
jit_call_external(JITCompiler* jit, void* func_ptr)
{
  // set up stack for external call
  jit_emit(jit, 0xd10043ff);               // sub sp, sp, #16
  jit_emit(jit, arm64_stp(29, 30, 31, 0)); // stp x29, x30, [sp]

  // the call
  int64_t offset = (int64_t)func_ptr - (int64_t)&jit->code[jit->code_size];
  jit_emit(jit, arm64_bl(offset / 4));

  // restore stack after call
  jit_emit(jit, arm64_ldp(29, 30, 31, 0)); // ldp x29, x30, [sp]
  jit_emit(jit, 0x910043ff);               // add sp, sp, #16
}

#endif // TINY_JIT_IMPLEMENTATION

#endif // __TINY_JIT_H

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
