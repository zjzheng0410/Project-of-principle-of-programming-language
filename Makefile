CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -g
CPPFLAGS ?= -Iruntime -Iprocessor

PROCESSOR_SRC := $(wildcard processor/*.c)
RUNTIME_SRC := $(wildcard runtime/*.c)
PROCESSOR_OBJ := $(patsubst %.c,build/%.o,$(PROCESSOR_SRC))
RUNTIME_OBJ := $(patsubst %.c,build/%.o,$(RUNTIME_SRC))

.PHONY: all clean test demo

all: build/pplc

build/pplc: $(PROCESSOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/processor/%.o: processor/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

build/runtime/%.o: runtime/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

generated/2026_feature_demo.c: build/pplc examples/2026_feature_demo.ppl
	@mkdir -p generated
	./build/pplc examples/2026_feature_demo.ppl -o generated/2026_feature_demo.c

build/2026_feature_demo: generated/2026_feature_demo.c $(RUNTIME_OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ generated/2026_feature_demo.c $(RUNTIME_OBJ)

demo: build/2026_feature_demo
	./build/2026_feature_demo

test: build/pplc
	bash tests/run_tests.sh

clean:
	rm -rf build generated/*.c generated/*.bin generated/test_*.c
