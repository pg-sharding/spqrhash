
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(text, text);
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(bytea, text);
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(text, text, int8);
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(bytea, text, int8);
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(text, text, int8, int8);
ALTER EXTENSION hashlib ADD FUNCTION hash64_string(bytea, text, int8, int8);
ALTER EXTENSION hashlib ADD FUNCTION hash_int8(int8, text);

