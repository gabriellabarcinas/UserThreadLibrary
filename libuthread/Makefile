# Target library
lib := libuthread.a

objs := queue.o uthread.o sem.o preempt.o context.o
CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g
PANDOC := pandoc

ifneq ($(V),1)
Q = @
endif

all: $(lib)

## Phase 1
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

libuthread.a: $(objs)
	@echo "CC $@"
	ar rcs $(lib) $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

%.html: %.md
	@echo "CC $@"
	$(Q)$(PANDOC) -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)
