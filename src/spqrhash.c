
#include "spqrhash.h"

#include <access/htup_details.h>

#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_hash64_string);
PG_FUNCTION_INFO_V1(pg_hash_int64);
PG_FUNCTION_INFO_V1(spqr_hash_murmur3_str);
PG_FUNCTION_INFO_V1(spqr_hash_murmur3_int64);

/*
 * Algorithm data
 */

#define HASHNAMELEN 12

struct StrHashDesc {
	int namelen;
	const char name[HASHNAMELEN];
	hlib_str_hash_fn hash;
	uint64_t initval;
};

struct Int64HashDesc {
	int namelen;
	const char name[HASHNAMELEN];
	hlib_int64_hash_fn hash;
};

static const struct StrHashDesc string_hash_list[] = {
	{ 7, "murmur3",		hlib_murmur3, 0 },
	{ 0 },
};

static const struct Int64HashDesc int64_hash_list[] = {
	{ 7, "murmur3",		hlib_murmur3_int64 },
	{ 0 },
};

/*
 * Lookup functions.
 */

static const struct StrHashDesc *
find_string_hash(const char *name, unsigned nlen)
{
	const struct StrHashDesc *desc;
	char buf[HASHNAMELEN];

	if (nlen >= HASHNAMELEN)
		return NULL;
	memset(buf, 0, sizeof(buf));
	memcpy(buf, name, nlen);

	for (desc = string_hash_list; desc->namelen; desc++) {
		if (desc->namelen != nlen)
			continue;
		if (name[0] != desc->name[0])
			continue;
		if (memcmp(desc->name, name, nlen) == 0)
			return desc;
	}
	return NULL;
}

static const struct Int64HashDesc *
find_int64_hash(const char *name, unsigned nlen)
{
	const struct Int64HashDesc *desc;
	char buf[HASHNAMELEN];

	if (nlen >= HASHNAMELEN)
		return NULL;
	memset(buf, 0, sizeof(buf));
	memcpy(buf, name, nlen);

	for (desc = int64_hash_list; desc->namelen; desc++) {
		if (desc->namelen == nlen && !memcmp(desc->name, name, nlen))
			return desc;
	}
	return NULL;
}

/*
 * Utility functions.
 */

static void
err_nohash(text *hashname)
{
	const char *name;
	name = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(hashname)));
	elog(ERROR, "hash '%s' not found", name);
}

/*
 * Public functions
 */

/* hash64_string(bytea, text [, int8 [, int8]]) returns int8 */
Datum
pg_hash64_string(PG_FUNCTION_ARGS)
{
	struct varlena *data;
	text *hashname = PG_GETARG_TEXT_PP(1);
	uint64_t io[MAX_IO_VALUES];
	const struct StrHashDesc *desc;

	memset(io, 0, sizeof(io));

	/* request aligned data on weird architectures */
#ifdef HLIB_UNALIGNED_READ_OK
	data = PG_GETARG_VARLENA_PP(0);
#else
	data = PG_GETARG_VARLENA_P(0);
#endif

	/* load hash */
	desc = find_string_hash(VARDATA_ANY(hashname), VARSIZE_ANY_EXHDR(hashname));
	if (desc == NULL)
		err_nohash(hashname);

	/* decide initvals */
	if (PG_NARGS() >= 4)
		io[1] = PG_GETARG_INT64(3);
	if (PG_NARGS() >= 3)
		io[0] = PG_GETARG_INT64(2);
	else
		io[0] = desc->initval;

	/* do hash */
	desc->hash(VARDATA_ANY(data), VARSIZE_ANY_EXHDR(data), io);

	PG_FREE_IF_COPY(data, 0);
	PG_FREE_IF_COPY(hashname, 1);

	PG_RETURN_INT64(io[0]);
}

/*
 * Integer hashing
 */

/* hash_int8(int8, text) returns int8 */
Datum
pg_hash_int64(PG_FUNCTION_ARGS)
{
	int64 data = PG_GETARG_INT64(0);
	text *hashname = PG_GETARG_TEXT_PP(1);
	const struct Int64HashDesc *desc;

	desc = find_int64_hash(VARDATA_ANY(hashname), VARSIZE_ANY_EXHDR(hashname));
	if (desc == NULL)
		err_nohash(hashname);
	PG_FREE_IF_COPY(hashname, 1);

	PG_RETURN_INT64(desc->hash((uint64_t)(data)));
}

Datum
spqr_hash_murmur3_str(PG_FUNCTION_ARGS) 
{
	struct varlena *data;
	uint64_t io[MAX_IO_VALUES];

	memset(io, 0, sizeof(io));

	/* request aligned data on weird architectures */
#ifdef HLIB_UNALIGNED_READ_OK
	data = PG_GETARG_VARLENA_PP(0);
#else
	data = PG_GETARG_VARLENA_P(0);
#endif

	/* do hash */
	hlib_murmur3(VARDATA_ANY(data), VARSIZE_ANY_EXHDR(data), io);

	PG_FREE_IF_COPY(data, 0);
	PG_FREE_IF_COPY(hashname, 1);

	PG_RETURN_INT64(io[0]);
}

Datum
spqr_hash_murmur3_int64(PG_FUNCTION_ARGS)
{
	int64 data = PG_GETARG_INT64(0);

	PG_RETURN_INT64(hlib_murmur3_int64((uint64_t)(data)));
}


