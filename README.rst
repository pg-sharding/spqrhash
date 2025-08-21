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

spqrhash_murmur3
~~~~~~~~~~~~~

::
  spqrhash_murmur3(text) RETURNS int8
  spqrhash_murmur3(bytea) RETURNS int8
  spqrhash_murmur3(int8) RETURNS int8

Murmur3 hash algorithm

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
