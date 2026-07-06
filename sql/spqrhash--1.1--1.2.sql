
CREATE OR REPLACE FUNCTION spqrhash_murmur3(id uuid)
RETURNS int8 AS
$$
SELECT public.spqrhash_murmur3(id::text)
$$
LANGUAGE SQL;

CREATE OR REPLACE FUNCTION spqrhash_city32(id uuid)
RETURNS int8 AS
$$
SELECT public.spqrhash_city32(id::text)
$$
LANGUAGE SQL;

