EXTENSION = pgchy_similarity
MODULE_big = pgchy_similarity
DATA = sql/pgchy_similarity--1.0.sql
OBJS = src/pgchy_similarity.o

PG_CONFIG = pg_config


# SHLIB_LINK += -llz4

# COPT += -O3 
# COPT += -msse4.2
COPT += -mavx2 


PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# CFLAGS += -I$(shell $(PG_CONFIG) --includedir-server) -I/usr/include/lz4 -I./pgchy_similarity
# CFLAGS += -I/usr/include
# CFLAGS += -I/code/version2
# SHLIB_LINK += -llz4

PG_CFLAGS += -I$(shell $(PG_CONFIG) --includedir-server) -O3 -msse4.2

install:
	$(MKDIR_P) '$(DESTDIR)$(shell $(PG_CONFIG) --pkglibdir)'
	$(INSTALL_SHLIB) pgchy_similarity.so '$(DESTDIR)$(shell $(PG_CONFIG) --pkglibdir)'
	$(MKDIR_P) '$(DESTDIR)$(shell $(PG_CONFIG) --sharedir)/extension'
	$(INSTALL_DATA) pgchy_similarity.control '$(DESTDIR)$(shell $(PG_CONFIG) --sharedir)/extension'
	$(INSTALL_DATA) sql/pgchy_similarity--1.0.sql '$(DESTDIR)$(shell $(PG_CONFIG) --sharedir)/extension'


tests: run_tests 


run_tests:
	gcc -I. -o test_fingerprint tests/test_fingerprint.c src/pgchy_similarity.c -lcmocka -llz4 -I/usr/include/postgresql/14/server -I/home/dzanadvo/projects/libpopcnt
	./test_fingerprint
	rm ./test_fingerprint


# CFLAGS += -I$(shell $(PG_CONFIG) --includedir-server) -I/usr/include/lz4 -I ~/projects/pgchy_similarity -I ~/projects/libpopcnt
# # Target to compile the benchmark executable
# benchmark: src/benchmark.c
# 	gcc -I/home/dzanadvo/projects/pgchy_similarity -I/home/dzanadvo/projects/libpopcnt -o benchmark src/benchmark.c src/similarity_impl.c

# Target to run the benchmark
run_benchmark: benchmark
	./benchmark

# Define .PHONY targets to avoid conflicts with files named install, tests, etc.
.PHONY: install tests benchmark run_benchmark
