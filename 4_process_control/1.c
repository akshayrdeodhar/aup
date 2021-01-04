#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void printenv(char **envp) {
		while (*envp) {
				printf("%s\n", *envp);
				envp++;
		}
}

int main(void) {
		extern char **environ;

		printenv(environ);

		printf("\n");

		if (putenv("PATH=/usr/bin") == -1) {
				perror("putenv");
				return errno;
		}

		printf("Changed PATH\n\n");

		printenv(environ);

		return 0;
}
