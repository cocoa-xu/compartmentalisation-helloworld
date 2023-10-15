override PURECAP_CFLAGS=-march=morello+c64 -mabi=purecap -Xclang -morello-vararg=new
override HYBRID_CFLAGS=-march=morello -Xclang -morello-vararg=new

override CFLAGS=-O0 -g
override CC=clang-morello
