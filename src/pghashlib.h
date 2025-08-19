#ifndef _PGHASHLIB_H_
#define _PGHASHLIB_H_

#include <postgres.h>
#include <fmgr.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#if !defined(PG_VERSION_NUM) || (PG_VERSION_NUM < 80300)
#error "PostgreSQL 8.3+ required"
#endif

/* sometimes <endian.h> comes via <sys/types.h> */
#ifndef le64toh
#include "compat-endian.h"
#endif

/*
 * Does architecture support unaligned reads?
 *
 * If not, we copy the data to aligned location, because not all
 * hash implementation are fully portable.
 */
#if defined(__i386__) || defined(__x86_64__)
#define HLIB_UNALIGNED_READ_OK
#endif

/* how many values in io array will be used, max */
#define MAX_IO_VALUES 2

/* hash function signatures */
typedef void     (*hlib_str_hash_fn)(const void *data, size_t len, uint64_t *io);
typedef uint64_t (*hlib_int64_hash_fn)(uint64_t data);

/* string hashes */
void hlib_murmur3(const void *data, size_t len, uint64_t *io);

/* integer hashes */
uint64_t hlib_murmur3_int64(uint64_t data);

Datum pg_hash64_string(PG_FUNCTION_ARGS);
Datum pg_hash_int64(PG_FUNCTION_ARGS);

#endif

