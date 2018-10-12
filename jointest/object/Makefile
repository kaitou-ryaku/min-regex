CC      := gcc
CFLAGS  := -std=c99 -O0 -Wall
CPP     := $(CC) $(CFLAGS) -E

ALL_C   := $(wildcard ../*.c)
ALL_H   := $(wildcard ../../include/*.h)
ALL_O   := $(patsubst ../%.c,%.o,$(ALL_C))
ALL_D   := $(patsubst ../%.c,%.d,$(ALL_C))

.PHONY: dummy
dummy: $(ALL_O)

%.o: ../%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: ../%.c $(ALL_H)
	$(CPP) -MM $< -MF $@

-include $(ALL_D)
