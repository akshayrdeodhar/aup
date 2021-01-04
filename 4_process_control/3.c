#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define N_CHILDREN 2
#define F_N 10


/* calcualte factorial using recursion */
long int factorial(int n) {
		long int f;
		if (n == 1) {
				f = 1;
		}
		else {
				f = n * factorial(n - 1);
		}
		return f;
}

void child_work(int child_number) {
		int i;
		/* print factorial values from n = 1 to 10 */
		for (i = 1; i <= F_N; i++) {
				printf("CHILD%d:fact(%d)=%ld\n", child_number, i, factorial(i));
		}
}

int main(void) {
		int i;
		int child;
		/* create children, and dispatch each child to calcualte and print
		 * factorials */
		for (i = 0; i < N_CHILDREN; i++) {
				if ((child = fork()) == -1) {
						/* fork failed */
						perror("unable to create child process");
				}
				else if (child == 0) {
						/* child */
						child_work(i + 1);
						return 0;
				}
		}

		return 0;
}


