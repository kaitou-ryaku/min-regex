CC      := gcc
CFLAGS  := -std=c99 -O0 -Wall

ARCHIVE  := min-regex.a
TARGET   := min-regex.out
JOINTEST := jointest.out

ALL_CH  := $(wildcard src/*.c include/*.h)
ALL_O   := $(patsubst src/%.c,object/%.o,$(wildcard src/*.c))
ALL_CH_WITHOUT_MAIN := $(filter-out src/main.c,$(ALL_CH))
ALL_O_WITHOUT_MAIN  := $(filter-out object/main.o,$(ALL_O))

dummy: $(TARGET)
test: $(JOINTEST)

$(ALL_O_WITHOUT_MAIN): $(ALL_CH_WITHOUT_MAIN)
	cd object && $(MAKE) $(patsubst object/%.o,%.o,$@) "CC=$(CC)" "CFLAGS=$(CFLAGS)"

$(ARCHIVE): $(ALL_O_WITHOUT_MAIN)
	ar -r $@ $^

$(TARGET): $(ARCHIVE) src/main.c
	cd object && $(MAKE) main.o "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) object/main.o $< -o $@

$(JOINTEST): $(ARCHIVE) jointest/jointest.c
	cd jointest/object && $(MAKE) jointest.o "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) jointest/object/jointest.o $< -o $@
#	./$@

.PHONY: clean
clean:
	rm -rf *.a *.out *.stackdump *.dot *.png
	cd object && rm -rf *.o *.d
	cd jointest/object && rm -rf *.o *.d
