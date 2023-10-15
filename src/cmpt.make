.DEFAULT_GOAL = cmpt-example

override cmpt_this := $(lastword $(MAKEFILE_LIST))
override util_src := util/capprint.c util/morello.c

override hellolb_src := $(util_src) src/lb.S src/manager.c src/main.c
override hellolpb_src := $(util_src) src/lpb.S src/manager.c src/main.c

override CFLAGS := $(CFLAGS) -Iutil

hellolb:
	$(CC) $(PURECAP_CFLAGS) $(CFLAGS) -DUSE_LB_SEALED_CAP $(hellolb_src) -o hellolb-purecap

hellolpb:
	$(CC) $(PURECAP_CFLAGS) $(CFLAGS) $(hellolpb_src) -o hellolpb-purecap

cmpt-example: hellolb hellolpb

clean-cmpt-example:
	@rm -f hellolb-purecap hellolpb-purecap
