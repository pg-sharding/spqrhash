#ifndef _SPQRHash_H_
#define _SPQRHash_H_

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

/* string hashes */
void hlib_murmur3(const void *data, size_t len, uint64_t *io);
void hlib_city32_str(const void *data, size_t len, uint64_t *io);

/* integer hashes */
uint64_t hlib_murmur3_int64(uint64_t data);
uint64_t hlib_city32_int64(uint64_t data);

Datum spqr_hash_murmur3_str(PG_FUNCTION_ARGS);
Datum spqr_hash_murmur3_int64(PG_FUNCTION_ARGS);
Datum spqr_hash_city32_str(PG_FUNCTION_ARGS);
Datum spqr_hash_city32_int64(PG_FUNCTION_ARGS);

#endif

