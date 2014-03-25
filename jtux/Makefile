JAVA_INCLUDE = /usr/lib/jvm/default-java/include
INCLUDE = -I. -I./include -I$(JAVA_INCLUDE) -I$(JAVA_INCLUDE)/linux
CFLAGS  = -Wall -O2 -D_REENTRANT -D_THREAD_SAFE -std=c99
OBJECTS = jtux_clock.o jtux_dir.o jtux_file.o jtux_network.o \
          jtux_posixipc.o jtux_process.o jtux_sysvipc.o jtux_util.o

.c.o:
	$(CC) $(CFLAGS) -fPIC -DLINUX $(INCLUDE) -c $(<) -o $(@)

all: $(OBJECTS)
	$(CC) -o libjtux.so -shared -lrt -lnsl $(OBJECTS)

clean:
	rm -f *.o

distclean: clean
	rm -f libjtux.so
