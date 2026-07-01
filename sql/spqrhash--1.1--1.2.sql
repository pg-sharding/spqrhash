
CREATE OR REPLACE FUNCTION spqrhash_murmur3(id uuid)
RETURNS int8 AS
$$
spqrhash_murmur3(id::text)
$$
LANGUAGE SQL;


