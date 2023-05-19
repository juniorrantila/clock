#pragma once

#define static_assert _Static_assert
typedef char const* c_string;

typedef signed char i8;
static_assert(sizeof(i8) == 1, "");
typedef unsigned char u8;
static_assert(sizeof(u8) == 1, "");

typedef signed short i16;
static_assert(sizeof(i16) == 2, "");
typedef unsigned short u16;
static_assert(sizeof(u16) == 2, "");

typedef signed int i32;
static_assert(sizeof(i32) == 4, "");

typedef unsigned int u32;
static_assert(sizeof(u32) == 4, "");

typedef signed long i64;
static_assert(sizeof(i64) == 8, "");

typedef unsigned long u64;
static_assert(sizeof(u64) == 8, "");

typedef signed __int128 i128;
static_assert(sizeof(i128) == 16, "");

typedef unsigned __int128 u128;
static_assert(sizeof(u128) == 16, "");

typedef __SIZE_TYPE__ usize;
#define unsigned // NOLINT
typedef __SIZE_TYPE__ isize;
#undef unsigned

typedef usize uptr;
static_assert(sizeof(usize) == sizeof(uptr), "");

typedef isize iptr;
static_assert(sizeof(isize) == sizeof(iptr), "");

typedef float f32;
typedef double f64;

#define null ((void*)0)
