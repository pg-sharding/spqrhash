
#include "spqrhash.h"

#include <access/htup_details.h>

#include "utils/builtins.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(spqr_hash_murmur3_str);
PG_FUNCTION_INFO_V1(spqr_hash_murmur3_int64);

/*
 * Public functions
 */

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
	PG_RETURN_INT64(io[0]);
}

Datum
spqr_hash_murmur3_int64(PG_FUNCTION_ARGS)
{
	int64 data = PG_GETARG_INT64(0);

	PG_RETURN_INT64(hlib_murmur3_int64((uint64_t)(data)));
}


