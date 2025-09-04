WARNINGS=-fstack-protector -pedantic -W -Wall -Wbad-function-cast \
	-Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels \
	-Wfloat-equal -Wformat=2 -Wformat-nonliteral -Winline \
	-Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
	-Wno-unused-parameter -Wpointer-arith -Wshadow -Wstrict-prototypes \
	-Wstack-protector -Wswitch -Wundef -Wwrite-strings

INCLUDES=
LDFLAGS=
CFLAGS=$(DEBUG) $(WARNINGS) $(INCLUDES) $(RELEASE) -std=c99
OBJS=foldr.o

all: foldr site

cron: update site


# Build the compiler
foldr: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

site:
	$(MAKE) -C ./src/

clean:
	rm -f *.o foldr
	rm -rf ./build/
	$(MAKE) -C ./src/ clean

serve:
	cd build/ && python3 -m http.server

synchttpdocs:


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
