spqrhash
=========

The extension provides hash funcitons for SPQR to work with PG.


Installation
------------

You need PostgreSQL developent environment.  Then simply::

  $ make
  $ make install
  $ psql -d ... -c "create extension spqrhash"


Functions
---------

hash64_string
~~~~~~~~~~~~~

::

  hash64_string(data text, algo text, [, iv1 int8 [, iv2 int8]]) returns int8
  hash64_string(data byte, algo text, [, iv1 int8 [, iv2 int8]]) returns int8

Uses same algorithms as `hash_string()` but returns 64-bit result.

hash_int8
~~~~~~~~~

::

  hash_int8(val int8) returns int8

Hash 64-bit integer.



Hashing algorithms
-------------------------

List of currently provided algorithms.

==============  =========  ======  =======  =======  ==============================
 Algorithm      CPU-indep   Bits   IV bits  Partial   Description
==============  =========  ======  =======  =======  ==============================
 murmur3         no          32       32       no      MurmurHash v3, 32-bit variant
==============  =========  ======  =======  =======  ==============================

CPU-independence
  Whether hash output is independent of CPU endianess.  If not, then
  hash result is different on little-endian machines (x86)
  and big-endian machines (sparc).

Bits
  Maximum number of output bits that hash can output.
  If longer result is requested, result will be
  zero-padded.

IV bits
  Maximum number of input bits for "initial value".

Partial hashing
  Whether long string can be hashed in smaller parts, by giving last
  value as initval to next hash call.

Links
-----

* `MurmurHash`__ by Austin Appleby.

.. __: http://code.google.com/p/smhasher/
