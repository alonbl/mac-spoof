CC = gcc
DEFS = -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE

all:	mac-spoof.so

clean:
	rm -f mac-spoof.so

mac-spoof.so:	\
		mac-spoof-hooks.c \
		mac-spoof.c \
		mac-spoof.version \
		$(NULL)
	$(CC) $(DEFS) $(CFLAGS) -shared -fPIC -fpic -o mac-spoof.so mac-spoof.c mac-spoof-hooks.c \
		-Wl,-soname,mac-poof.so -Wl,--error-unresolved-symbols -Wl,--version-script=mac-spoof.version $(LDFLAGS) -ldl

