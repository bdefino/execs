BUILD := build
CC := cc
CFLAGS := -z execstack
DEST := execs
SRCS := execs.c

.PHONY: all

all:
	[ -e $(BUILD) ] || mkdir $(BUILD)
	$(CC) $(CFLAGS) $(SRCS) -o $(BUILD)/$(DEST)

