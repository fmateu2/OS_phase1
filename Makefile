CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
TARGET  = maester
SRCDIR  = src
SRCS    = $(SRCDIR)/main.c \
          $(SRCDIR)/config.c \
          $(SRCDIR)/inventory.c \
          $(SRCDIR)/commands.c \
          $(SRCDIR)/trade.c \
          $(SRCDIR)/utils.c \
          $(SRCDIR)/signals.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean westeros

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRCDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/maester.h
	$(CC) $(CFLAGS) -c $< -o $@

westeros:
	@echo Peace

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
