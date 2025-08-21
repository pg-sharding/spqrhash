CREATE OR REPLACE FUNCTION hash64_string(text, text) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash64_string(bytea, text) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash64_string(text, text, int8) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash64_string(bytea, text, int8) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash64_string(text, text, int8, int8) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash64_string(bytea, text, int8, int8) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash64_string' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hash_int8(int8, text) RETURNS int8
	AS '$libdir/spqrhash', 'pg_hash_int64' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_murmur3(int8) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_int64' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_murmur3(text) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_str' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION spqrhash_murmur3(bytea) RETURNS int8
	AS '$libdir/spqrhash', 'spqr_hash_murmur3_str' LANGUAGE C IMMUTABLE STRICT;
