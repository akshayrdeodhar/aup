.PHONY: 1 2

1: 1.c 1_child1 1_child2
	clang 1.c -Wall -o 1

1_child1: 1_child1.c
	clang 1_child1.c -Wall -o 1_child1

1_child2: 1_child2.c
	clang 1_child2.c -Wall -o 1_child2

2: 2.c
	clang 2.c -Wall -o 2

3: 3.c
	clang 3.c -Wall -o 3


