CC = gcc
CFLAGS=-Wall -Wextra

all:
	$(CC) $(CFLAGS) 4.1/SimpleProgramL1.c 4.1/L1Cache.c -o 4.1/L1Cache
	$(CC) $(CFLAGS) 4.2/SimpleProgramL2.c 4.2/L2Cache.c -o 4.2/L2Cache
	$(CC) $(CFLAGS) 4.3/SimpleProgramL22W.c 4.3/L2Cache2W.c -o 4.3/L2Cache2W

test: all
	./4.1/L1Cache > tests/o1.txt
	diff tests/o1.txt tests/results_L1.txt
	./4.2/L2Cache > tests/o2.txt
	diff tests/o2.txt tests/results_L2_1W.txt
	./4.3/L2Cache2W > tests/o22w.txt
	diff tests/o22w.txt tests/results_L2_2W.txt



clean:
	rm 4.1/L1Cache
	rm 4.2/L2Cache
	rm 4.3/L22WCache