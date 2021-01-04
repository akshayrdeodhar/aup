1: 1.c
	clang 1.c -Wall -o 1

2: 2.c
	clang 2.c -Wall -o 2

3: 3.c
	clang 3.c -Wall -lpthread -o 3

111703013_a10.pdf: a10.md
	pandoc -o 111703013_a10.pdf a10.md --highlight-style tango

clean:
	rm /tmp/aup_fifo1 /tmp/aup_fifo2 /tmp/aup_file
