CC      := gcc
CFLAGS  := -std=c99 -O0 -Wall

TARGET  := min-regex.out
ALL_C   := $(wildcard src/*.c)
ALL_CH  := $(wildcard src/*.c include/*.h)
ALL_O   := $(patsubst src/%.c,object/%.o,$(ALL_C))

TARGET_T := jointest.out
ALL_CT   := $(wildcard jointest/*.c)
ALL_OT   := $(patsubst jointest/%.c,jointest/object/%.o,$(ALL_CT))
ALL_TO   := $(filter-out object/main.o,$(ALL_O) $(patsubst jointest/%.c,jointest/object/%.o,$(ALL_CT)))

dummy: $(ALL_CH) $(ALL_CT)
	cd object && $(MAKE) "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) $(ALL_O) -o $(TARGET)
	cd jointest/object && $(MAKE) "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) $(ALL_TO) -o $(TARGET_T)
#	./jointest.out

notest: $(ALL_CH)
	cd object && $(MAKE) "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) $(ALL_O) -o $(TARGET)

.PHONY: clean
clean:
	@rm -rf *.out *.stackdump *.dot *.png object/*.o object/*.d jointest/object/*.o jointest/object/*.d
