CFLAGS += -Wall

all: scanWifi

scanWifi: scanWifi.o
	$(CC) $(CFLAGS) -o scanWifi scanWifi.o

scanWifi.o: scanWifi.c

clean:
	rm -f scanWifi scanWifi.o
