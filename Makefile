all: playground

include config.make
include src/cmpt.make

example: cmpt-example

clean: clean-cmpt-example

.PHONY: clean all
