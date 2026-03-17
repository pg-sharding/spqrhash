
CREATE OR REPLACE FUNCTION spqrhash_murmur3(int8[]) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_int64_arr' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_city32(int8[]) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_city32_int64_arr' LANGUAGE C IMMUTABLE STRICT;
