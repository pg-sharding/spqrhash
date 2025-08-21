
CREATE OR REPLACE FUNCTION spqrhash_murmur3(int8) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_int64' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_murmur3(text) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_str' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_murmur3(bytea) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_str' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_city32(int8) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_city32_int64' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_city32(text) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_city32_str' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_city32(bytea) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_city32_str' LANGUAGE C IMMUTABLE STRICT;
