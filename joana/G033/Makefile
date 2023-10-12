CC = gcc
CFLAGS=-Wall -Wextra

all:
	$(CC) $(CFLAGS) 4.1/SimpleProgramL1.c 4.1/L1Cache.c -o 4.1/L1Cache
	$(CC) $(CFLAGS) 4.2/SimpleProgramL2.c 4.2/L2Cache.c -o 4.2/L2Cache
	$(CC) $(CFLAGS) 4.3/SimpleProgramL2_2W.c 4.3/L2Cache2W.c -o 4.3/L2Cache2W

test: all
	./4.1/L1Cache > tests/outputs/L1Cache_test.txt
	./4.2/L2Cache > tests/outputs/L2Cache_test.txt
	./4.3/L2Cache2W > tests/outputs/L2Cache2W_test.txt

	diff tests/outputs/L1Cache_test.txt tests/results/results_L1.txt
	diff tests/outputs/L2Cache_test.txt tests/results/results_L2_1W.txt
	diff tests/outputs/L2Cache2W_test.txt tests/results/results_L2_2W.txt

clean:
	rm 4.1/L1Cache 4.2/L2Cache 4.3/L2Cache2W