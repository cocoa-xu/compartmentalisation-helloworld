.DEFAULT_GOAL = example

CC ?= clang-morello
PURECAP_CFLAGS = -march=morello+c64 -mabi=purecap -Xclang -morello-vararg=new
HYBRID_CFLAGS = -march=morello -Xclang -morello-vararg=new

SRC = $(shell pwd)/src
UTIL = $(shell pwd)/util
UTIL_SRC := $(UTIL)/capprint.c $(UTIL)/morello.c

LB_SRC := $(UTIL_SRC) src/lb.S main.c
LPB_SRC := $(UTIL_SRC) src/lpb.S main.c

hellolb: $(UTIL_SRC) src/lb.S main.c
	$(CC) $(PURECAP_CFLAGS) $(CFLAGS) -Iutil -DUSE_LB_SEALED_CAP $(LB_SRC) -o hellolb-purecap

hellolpb: $(UTIL_SRC) src/lpb.S main.c
	$(CC) $(PURECAP_CFLAGS) $(CFLAGS) -Iutil $(LPB_SRC) -o hellolpb-purecap

example: hellolb hellolpb
	@echo > /dev/null
