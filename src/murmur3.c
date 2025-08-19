//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

// Note - The x86 and x64 versions do _not_ produce the same results, as the
// algorithms are optimized for their respective platforms. You can still
// compile and run any of them on any platform, but your performance with the
// non-native version will be less than optimal.

#include "pghashlib.h"


const int ENCODING_BYTES_BIG = 10;
const int ENCODING_BYTES = 8;
const uint64_t BOUND = 1UL << 56;

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

static inline uint32_t rotl32(uint32_t x, int8_t r)
{
	 return (x << r) | (x >> (32 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here
static inline uint32_t getblock(const uint32_t * p, int i)
{
	 return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche
static inline uint32_t fmix(uint32_t h)
{
	 h ^= h >> 16;
	 h *= 0x85ebca6b;
	 h ^= h >> 13;
	 h *= 0xc2b2ae35;
	 h ^= h >> 16;
	 return h;
}

//-----------------------------------------------------------------------------
void hlib_murmur3(const void *key, size_t len, uint64_t *io)
{
	 const uint8_t *data = (const uint8_t *) key;
	 const int nblocks = len / 4;
	 uint32_t h1 = io[0];
	 uint32_t c1 = 0xcc9e2d51;
	 uint32_t c2 = 0x1b873593;
	 const uint32_t *blocks;
	 const uint8_t *tail;
	 int i;
	 uint32_t k1;

	 //----------
	 // body
	 blocks = (const uint32_t *) (data + nblocks * 4);
	 for (i = -nblocks; i; i++) {
		  k1 = getblock(blocks, i);
		  k1 *= c1;
		  k1 = ROTL32(k1, 15);
		  k1 *= c2;
		  h1 ^= k1;
		  h1 = ROTL32(h1, 13);
		  h1 = h1 * 5 + 0xe6546b64;
	 }
	 //----------
	 // tail
	 tail = (const uint8_t *) (data + nblocks * 4);
	 k1 = 0;
	 switch (len & 3) {
	 case 3:
		  k1 ^= tail[2] << 16;
		  /* fall through */
	 case 2:
		  k1 ^= tail[1] << 8;
		  /* fall through */
	 case 1:
		  k1 ^= tail[0];
		  k1 *= c1;
		  k1 = ROTL32(k1, 15);
		  k1 *= c2;
		  h1 ^= k1;
	 };

	 //----------
	 // finalization
	 h1 ^= len;
	 h1 = fmix(h1);
	 io[0] = h1;
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

uint64_t hlib_murmur3_int64(uint64_t input_data)
{
	 size_t len;
	 uint8_t *key;
	 uint64_t io[MAX_IO_VALUES];
	 int sz = ENCODING_BYTES;
	 if (input_data >= BOUND) {
		sz = ENCODING_BYTES_BIG;
	 }
	 key = alloca(sz * sizeof *key);
	 len = sz;
	 memset(key, 0, len);
	 put_uvarint(key, input_data);
	 memset(io, 0, sizeof(io));
	 hlib_murmur3(key, len, io);
	 return (uint64_t)(uint32_t)io[0];
}

