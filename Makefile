CC      := gcc
CFLAGS  := -std=c99 -O0 -Wall

TARGET  := min-regex.out
ALL_C   := $(wildcard src/*.c)
ALL_CH  := $(wildcard src/*.c include/*.h)
ALL_O   := $(patsubst src/%.c,object/%.o,$(ALL_C))

dummy: $(ALL_CH) jointest/jointest.c
	@cd object && $(MAKE) "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) $(ALL_O) -o $(TARGET)
	@cp object/*.o jointest/object/
	@rm jointest/object/main.o
	$(CC) $(CFLAGS) -c jointest/jointest.c -o jointest/object/jointest.o
	$(CC) $(CFLAGS) $(wildcard jointest/object/*.o) -o jointest.out

notest: $(ALL_CH)
	@cd object && $(MAKE) "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) $(ALL_O) -o $(TARGET)

.PHONY: clean
clean:
	@rm -rf *.out *.stackdump tmp* object/*.o object/*.d jointest/object/*.o jointest/object/*.out
