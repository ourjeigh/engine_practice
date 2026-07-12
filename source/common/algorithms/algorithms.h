#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__
#pragma once

#include "types/types.h"
#include "structures/array.h"

// note that if passing in an object to be hashed, the caller is
// responsible for ensuring there are no garbage bytes in the padding
// as they could results in two equal objects not having equal hashes.
constexpr uint32 fnv1_hash_32(const void* input, uint64 size);
constexpr uint32 fnv1a_hash_32(const void* input, uint64 size);
constexpr uint64 fnv1_hash_64(const void* input, uint64 size);
constexpr uint64 fnv1a_hash_64(const void* input, uint64 size);

constexpr uint32 fnv1_string_hash_32(const char* input, uint64 size);
constexpr uint32 fnv1a_string_hash_32(const char* input, uint64 size);
constexpr uint64 fnv1_string_hash_64(const char* input, uint64 size);
constexpr uint64 fnv1a_string_hash_64(const char* input, uint64 size);

#define MAKE_STRING_HASH(object) fnv1a_string_hash_64(object, sizeof(object))

#include "algorithms_hashing.inl"

// return true to sort left before right
typedef bool(*f_comparitor)(const void* left, const void* right/*, const void* context*/);

// use quick sort because it has "quick" in the name
template<typename t_type>
void quick_sort(c_array<t_type> array, f_comparitor comparitor, int32 left, int32 right);

#include "algorithms/algorithms_sort.inl"

#endif //__ALGORITHMS_H__
