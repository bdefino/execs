BUILD := build
CC := cc
CFLAGS := -z execstack
DEST := execs
SRCS := execs.c

.PHONY: clean build

build:
	[ -e $(BUILD) ] || mkdir $(BUILD)
	$(CC) $(CFLAGS) $(SRCS) -o $(BUILD)/$(DEST)

clean:
	[ -e $(BUILD) ] && rm -r $(BUILD)

