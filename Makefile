# configurable variables
PG_CONFIG = pg_config
RST2HTML = rst2html


ifdef USE_PGXS
# launch PGXS
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/spqrhash
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

# module description
MODULE_big = spqrhash
SRCS = src/spqrhash.c src/murmur3.c src/city.c
OBJS = $(SRCS:.c=.o)
EXTENSION = $(MODULE_big)

DOCS = spqrhash.html
EXTRA_CLEAN = spqrhash.html

REGRESS_OPTS = --inputdir=test

# different vars for extension and plain module

Regress_noext = test_init_noext test_int8_murmur test_string_murmur test_int8_arr_murmur test_int8_arr_murmur3_many_args test_string_city32 test_string_varlen_city32 test_int8_city32 test_int8_arr_city32 test_int8_arr_city32_many_args
Regress_ext   = test_init_ext   test_upgrade_ext test_int8_murmur test_string_murmur test_int8_arr_murmur test_int8_arr_murmur3_many_args test_string_city32 test_string_varlen_city32 test_int8_city32 test_int8_arr_city32 test_int8_arr_city32_many_args

Data_noext = sql/spqrhash.sql sql/uninstall_spqrhash.sql
Data_ext = sql/spqrhash--1.0.sql sql/spqrhash--1.0--1.1.sql sql/spqrhash--1.1.sql sql/spqrhash--unpackaged--1.0.sql sql/spqrhash--unpackaged--1.0--1.1.sql sql/spqrhash--unpackaged--1.1.sql

# Work around PGXS deficiencies - switch variables based on
# whether extensions are supported.
PgMajor = $(if $(MAJORVERSION),$(MAJORVERSION),15)
PgHaveExt = $(if $(filter 8.% 9.0,$(PgMajor)),noext,ext)
DATA = $(Data_$(PgHaveExt))
REGRESS = $(Regress_$(PgHaveExt))


install: $(DOCS)

test: install
	make installcheck || { filterdiff --format=unified regression.diffs | less; exit 1; }

ack:
	cp results/*.out test/expected/

tags:
	cscope -I . -b -f .cscope.out src/*.c

%.s: %.c
	$(CC) -S -fverbose-asm -o - $< $(CFLAGS) $(CPPFLAGS) | cleanasm > $@

html: spqrhash.html

spqrhash.html: README.rst
	$(RST2HTML) $< > $@

deb:
	rm -f debian/control
	make -f debian/rules debian/control
	debuild -uc -us -b

debclean: clean
	$(MAKE) -f debian/rules realclean
	rm -f lib* spqrhash.so* spqrhash.a
	rm -rf .deps
	rm -rf build-*

pg_version?=16
codename?=jammy
regress:
	docker build . --tag spqrhash_regress:1.0 --build-arg POSTGRES_VERSION=$(pg_version) --build-arg codename=$(codename) && docker run spqrhash_regress:1.0 | tee logs.out

