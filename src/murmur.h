/*
 * MurmurHash2, 64-bit versions, by Austin Appleby
 * --
 * The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
 * and endian-ness issues if used across multiple platforms.
 */

#ifndef MURMUR_H
#define MURMUR_H

uint32_t MurmurHash2(const void *key, int32_t len, uint32_t seed);
uint64_t MurmurHash64B(const void *key, int32_t len, uint32_t seed);

#endif
