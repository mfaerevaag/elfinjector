CC = gcc
CFLAGS = -Wall -std=gnu99
CLIBS =

SDIR = src
ODIR = build

SRC = $(wildcard $(SDIR)/*.c)
OBJ = $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(SRC))

PAYLOAD = payload

TARGET = encexpl

.PHONY: all debug release payload clean

all: debug

$(ODIR):
	mkdir -p $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $(CLIBS) -c $< -o $@

$(ODIR)/$(TARGET): $(ODIR) $(OBJ)
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $(OBJ)

release: CFLAGS += -O3
release: $(ODIR)/$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(ODIR)/$(TARGET)

payload: $(PAYLOAD).s
	nasm -f elf64 -o $(ODIR)/$(PAYLOAD).o $<
	ld -o $(ODIR)/$(PAYLOAD) $(ODIR)/$(PAYLOAD).o

clean:
	rm -rf $(ODIR)/*
