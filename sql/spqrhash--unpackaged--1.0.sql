
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(text, text);
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(bytea, text);
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(text, text, int8);
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(bytea, text, int8);
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(text, text, int8, int8);
ALTER EXTENSION spqrhash ADD FUNCTION hash64_string(bytea, text, int8, int8);
ALTER EXTENSION spqrhash ADD FUNCTION hash_int8(int8, text);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(int8);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(text);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(bytea);