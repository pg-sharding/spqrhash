// Copyright (c) 2011 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// CityHash, by Geoff Pike and Jyrki Alakuijala
//
// This file provides a few functions for hashing strings. On x86-64
// hardware in 2011, CityHash64() is faster than other high-quality
// hash functions, such as Murmur.  This is largely due to higher
// instruction-level parallelism.  CityHash64() and CityHash128() also perform
// well on hash-quality tests.
//
// CityHash128() is optimized for relatively long strings and returns
// a 128-bit hash.  For strings more than about 2000 bytes it can be
// faster than CityHash64().
//
// Functions in the CityHash family are not suitable for cryptography.
//
// WARNING: This code has not been tested on big-endian platforms!
// It is known to work well on little-endian platforms that have a small penalty
// for unaligned reads, such as current Intel and AMD moderate-to-high-end CPUs.
//
// By the way, for some hash functions, given strings a and b, the hash
// of a+b is easily derived from the hashes of a and b.  This property
// doesn't hold for any hash functions in this file.
//
// It's probably possible to create even faster hash functions by
// writing a program that systematically explores some of the space of
// possible hash functions, by using SIMD instructions, or by
// compromising on hash quality.

#include "spqrhash.h"
#include <byteswap.h>

#if defined __GNUC__ && __GNUC__ >= 3
#define HAVE_BUILTIN_EXPECT 1
#endif

struct city_uint128 {
	uint64_t first;
	uint64_t second;
};
typedef struct city_uint128 city_uint128;

static inline uint64_t Uint128Low64(const city_uint128 x) { return x.first; }
static inline uint64_t Uint128High64(const city_uint128 x) { return x.second; }

#define uint32_in_expected_order(x) le32toh(x)
#define uint64_in_expected_order(x) le64toh(x)

#if !defined(LIKELY)
#if HAVE_BUILTIN_EXPECT
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define LIKELY(x) (x)
#endif
#endif

static uint64_t UNALIGNED_LOAD64(const char *p)
{
	uint64_t result;
	memcpy(&result, p, sizeof(result));
	return result;
}

static uint32_t UNALIGNED_LOAD32(const char *p)
{
	uint32_t result;
	memcpy(&result, p, sizeof(result));
	return result;
}

static inline uint64_t Fetch64(const char *p)
{
	return uint64_in_expected_order(UNALIGNED_LOAD64(p));
}

static inline uint32_t Fetch32(const char *p)
{
	return uint32_in_expected_order(UNALIGNED_LOAD32(p));
}

// Magic numbers for 32-bit hashing.  Copied from Murmur3.
static const uint32_t c1 = 0xcc9e2d51;
static const uint32_t c2 = 0x1b873593;


// Some primes between 2^63 and 2^64 for various uses.
#define k0 0xc3a5c85c97cb3127ULL
#define k1 0xb492b66fbe98f273ULL
#define k2 0x9ae16a3b2f90404fULL
#define k3 0xc949d7c7509e6557ULL

static inline uint32_t Rotate32(uint32_t val, int shift) {
  // Avoid shifting by 32: doing so yields an undefined result.
  return shift == 0 ? val : ((val >> shift) | (val << (32 - shift)));
}

// A 32-bit to 32-bit integer hash copied from Murmur3.
static inline uint32_t fmix(uint32_t h)
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

static uint32_t Mur(uint32_t a, uint32_t h) {
  // Helper from Murmur3 for combining two 32-bit values.
  a *= c1;
  a = Rotate32(a, 17);
  a *= c2;
  h ^= a;
  h = Rotate32(h, 19);
  return h * 5 + 0xe6546b64;
}

static inline uint32_t Hash32Len0to4(const char *s, size_t len) {
  uint32_t b = 0;
  uint32_t c = 9;
  for (size_t i = 0; i < len; i++) {
    signed char v = (signed char)(s[i]);
    b = b * c1 + (uint32_t)(v);
    c ^= b;
  }
  return fmix(Mur(b, Mur((uint32_t)(len), c)));
}

static inline uint32_t Hash32Len5to12(const char *s, size_t len) {
  uint32_t a = (uint32_t)(len), b = a * 5, c = 9, d = b;
  a += Fetch32(s);
  b += Fetch32(s + len - 4);
  c += Fetch32(s + ((len >> 1) & 4));
  return fmix(Mur(c, Mur(b, Mur(a, d))));
}

static inline uint32_t Hash32Len13to24(const char *s, size_t len) {
  uint32_t a = Fetch32(s - 4 + (len >> 1));
  uint32_t b = Fetch32(s + 4);
  uint32_t c = Fetch32(s + len - 8);
  uint32_t d = Fetch32(s + (len >> 1));
  uint32_t e = Fetch32(s);
  uint32_t f = Fetch32(s + len - 4);
  uint32_t h = (uint32_t)(len);

  return fmix(Mur(f, Mur(e, Mur(d, Mur(c, Mur(b, Mur(a, h)))))));
}

static uint32_t CityHash32(const char *s, size_t len) {
  if (len <= 24) {
    return len <= 12 ?
        (len <= 4 ? Hash32Len0to4(s, len) : Hash32Len5to12(s, len)) :
        Hash32Len13to24(s, len);
  }

  // len > 24
  uint32_t h = (uint32_t)(len), g = c1 * h, f = g, tmp = 0;
  uint32_t a0 = Rotate32(Fetch32(s + len - 4) * c1, 17) * c2;
  uint32_t a1 = Rotate32(Fetch32(s + len - 8) * c1, 17) * c2;
  uint32_t a2 = Rotate32(Fetch32(s + len - 16) * c1, 17) * c2;
  uint32_t a3 = Rotate32(Fetch32(s + len - 12) * c1, 17) * c2;
  uint32_t a4 = Rotate32(Fetch32(s + len - 20) * c1, 17) * c2;
  h ^= a0;
  h = Rotate32(h, 19);
  h = h * 5 + 0xe6546b64;
  h ^= a2;
  h = Rotate32(h, 19);
  h = h * 5 + 0xe6546b64;
  g ^= a1;
  g = Rotate32(g, 19);
  g = g * 5 + 0xe6546b64;
  g ^= a3;
  g = Rotate32(g, 19);
  g = g * 5 + 0xe6546b64;
  f += a4;
  f = Rotate32(f, 19);
  f = f * 5 + 0xe6546b64;
  size_t iters = (len - 1) / 20;
  do {
    uint32_t a0 = Rotate32(Fetch32(s) * c1, 17) * c2;
    uint32_t a1 = Fetch32(s + 4);
    uint32_t a2 = Rotate32(Fetch32(s + 8) * c1, 17) * c2;
    uint32_t a3 = Rotate32(Fetch32(s + 12) * c1, 17) * c2;
    uint32_t a4 = Fetch32(s + 16);
    h ^= a0;
    h = Rotate32(h, 18);
    h = h * 5 + 0xe6546b64;
    f += a1;
    f = Rotate32(f, 19);
    f = f * c1;
    g += a2;
    g = Rotate32(g, 18);
    g = g * 5 + 0xe6546b64;
    h ^= a3 + a1;
    h = Rotate32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a4;
    g = bswap_32(g) * 5;
    h += a4 * 5;
    h = bswap_32(h);
    f += a0;
    tmp = f;
    f = g;
    g = h;
    h = tmp;
    s += 20;
  } while (--iters != 0);
  g = Rotate32(g, 11) * c1;
  g = Rotate32(g, 17) * c1;
  f = Rotate32(f, 11) * c1;
  f = Rotate32(f, 17) * c1;
  h = Rotate32(h + g, 19);
  h = h * 5 + 0xe6546b64;
  h = Rotate32(h, 17) * c1;
  h = Rotate32(h + f, 19);
  h = h * 5 + 0xe6546b64;
  h = Rotate32(h, 17) * c1;
  return h;
}

// Hash 128 input bits down to 64 bits of output.
// This is intended to be a reasonably good hash function.
static inline uint64_t Hash128to64(uint64_t u, uint64_t v)
{
	uint64_t a, b, kMul;
	// Murmur-inspired hashing.
	kMul = 0x9ddfea08eb382d69ULL;
	a = (u ^ v) * kMul;
	a ^= (a >> 47);
	b = (v ^ a) * kMul;
	b ^= (b >> 47);
	b *= kMul;
	return b;
}

/*
 * pghashlib API
 */

void hlib_city32_str(const void *s, size_t len, uint64_t *io) {
    io[0] = CityHash32(s, len);
}

/* Copied from go encoding/binary PutUVarInt func */
static int put_uvarint(uint8_t *buf, uint64_t n) {
	int i = 0;
	while (n >= 0x80) {
		buf[i] = (uint8_t)(n) | 0x80;
		n >>= 7;
		i++;
	}
	buf[i] = (uint8_t)(n);
	return i+1;
}

uint64_t hlib_city32_int64(uint64_t input_data)
{
    const int ENCODING_BYTES_BIG = 10;
    const int ENCODING_BYTES = 8;
    const uint64_t BOUND = 1UL << 56;
    size_t len;
    uint8_t *key;
    int sz = ENCODING_BYTES;
    if (input_data >= BOUND) {
    sz = ENCODING_BYTES_BIG;
    }
    key = alloca(sz * sizeof *key);
    len = sz;
    memset(key, 0, len);
    put_uvarint(key, input_data);
    return CityHash32(key, len);
}

