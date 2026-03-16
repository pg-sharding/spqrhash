
#include "spqrhash.h"

#include <access/htup_details.h>

#include "utils/builtins.h"
#include "utils/array.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(spqr_hash_murmur3_str);
PG_FUNCTION_INFO_V1(spqr_hash_murmur3_int64);
PG_FUNCTION_INFO_V1(spqr_hash_murmur3_int64_arr);
PG_FUNCTION_INFO_V1(spqr_hash_city32_str);
PG_FUNCTION_INFO_V1(spqr_hash_city32_int64);

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

Datum
spqr_hash_murmur3_int64_arr(PG_FUNCTION_ARGS)
{
	size_t array_size;
	size_t data_size;
	uint64_t io[MAX_IO_VALUES];
	uint8_t *data;
	int64 *elems;
	ArrayType *input_arr = PG_GETARG_ARRAYTYPE_P(0);
	

	memset(io, 0, sizeof(io));


	elems = (int32 *) ARR_DATA_PTR(input_arr);
	array_size = ArrayGetNItems(ARR_NDIM(input_arr),
						ARR_DIMS(input_arr));

	// TODO: check number size
	data_size = array_size * 8;
	data = alloca(data_size * sizeof *data);
	memset(data, 0, array_size * 8);
	for (size_t i = 0; i < array_size; i++) {
		uint64_t el = *(elems+i);
		put_uvarint(data+i*sizeof(uint64_t), el);
	}
	
	hlib_murmur3(data, data_size, io);

	PG_RETURN_INT64(io[0]);
}

Datum
spqr_hash_city32_str(PG_FUNCTION_ARGS) 
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
	hlib_city32_str(VARDATA_ANY(data), VARSIZE_ANY_EXHDR(data), io);

	PG_FREE_IF_COPY(data, 0);
	PG_RETURN_INT64(io[0]);
}

Datum
spqr_hash_city32_int64(PG_FUNCTION_ARGS)
{
	PG_RETURN_INT64(hlib_city32_int64((uint64_t)(PG_GETARG_INT64(0))));
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

