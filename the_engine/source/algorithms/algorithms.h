#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__
#pragma once

#include "types/types.h"

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
#endif //__ALGORITHMS_H__
