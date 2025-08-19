# configurable variables
PG_CONFIG = pg_config
RST2HTML = rst2html

# load version
include hashlib.control
EXT_VERSION = $(patsubst '%',%,$(default_version))
DISTNAME = pghashlib-$(EXT_VERSION)

# module description
MODULE_big = hashlib
SRCS = src/pghashlib.c src/murmur3.c src/city.c 
OBJS = $(SRCS:.c=.o)
EXTENSION = $(MODULE_big)

DOCS = hashlib.html
EXTRA_CLEAN = hashlib.html

REGRESS_OPTS = --inputdir=test

# different vars for extension and plain module

Regress_noext = test_init_noext test_int8_murmur
Regress_ext   = test_init_ext   test_int8_murmur

Data_noext = sql/hashlib.sql sql/uninstall_hashlib.sql
Data_ext = sql/hashlib--1.0.sql sql/hashlib--unpackaged--1.0.sql \
	   sql/hashlib--1.1.sql sql/hashlib--unpackaged--1.1.sql \
	   sql/hashlib--1.0--1.1.sql

# Work around PGXS deficiencies - switch variables based on
# whether extensions are supported.
PgMajor = $(if $(MAJORVERSION),$(MAJORVERSION),8.3)
PgHaveExt = $(if $(filter 8.% 9.0,$(PgMajor)),noext,ext)
DATA = $(Data_$(PgHaveExt))
REGRESS = $(Regress_$(PgHaveExt))


# launch PGXS
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

install: $(DOCS)

test: install
	make installcheck || { filterdiff --format=unified regression.diffs | less; exit 1; }

ack:
	cp results/*.out test/expected/

tags:
	cscope -I . -b -f .cscope.out src/*.c

%.s: %.c
	$(CC) -S -fverbose-asm -o - $< $(CFLAGS) $(CPPFLAGS) | cleanasm > $@

html: hashlib.html

hashlib.html: README.rst
	$(RST2HTML) $< > $@

deb:
	rm -f debian/control
	make -f debian/rules debian/control
	debuild -uc -us -b

debclean: clean
	$(MAKE) -f debian/rules realclean
	rm -f lib* hashlib.so* hashlib.a
	rm -rf .deps

tgz:
	git archive --prefix=$(DISTNAME)/ HEAD | gzip -9 > $(DISTNAME).tar.gz

pg_version?=16
codename?=jammy
regress:
	docker build . --tag pg_hash:1.0 --build-arg POSTGRES_VERSION=$(pg_version) --build-arg codename=$(codename) && docker run pg_hash:1.0 | tee logs.out

