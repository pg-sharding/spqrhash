
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(int8);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(text);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_murmur3(bytea);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_city32(text);
ALTER EXTENSION spqrhash ADD FUNCTION spqrhash_city32(bytea);