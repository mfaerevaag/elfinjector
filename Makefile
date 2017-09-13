CC = gcc
CFLAGS = -Wall -std=gnu99
CLIBS =

SDIR = src
ODIR = build

SRC = $(wildcard $(SDIR)/*.c)
OBJ = $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(SRC))

DUMMY = dummy
PAYLOAD = payload

TARGET = elfinjector

.PHONY: all debug release clean dummy payload

all: dummy payload debug

$(ODIR):
	mkdir -p $@

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $(CLIBS) -c $< -o $@

$(ODIR)/$(TARGET): $(ODIR) $(OBJ)
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $(OBJ)

release: CFLAGS += -O3
release: $(ODIR)/$(TARGET) $(ODIR)

debug: CFLAGS += -g -DDEBUG
debug: $(ODIR)/$(TARGET) $(ODIR)

payload: $(PAYLOAD).s $(ODIR)
	nasm -f elf64 -o $(ODIR)/$(PAYLOAD).o $<
	ld -o $(ODIR)/$(PAYLOAD) $(ODIR)/$(PAYLOAD).o

dummy: $(DUMMY).c $(ODIR)
	$(CC) -O0 -no-pie -o $(ODIR)/$(DUMMY) $<

clean:
	rm -rf $(ODIR)/*

.SPECIAL: $(ODIR)/$(DUMMY)
