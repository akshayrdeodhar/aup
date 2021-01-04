#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
		int fp;

		if (argc != 2) {
				fprintf(stderr, "usage: ./tryopen.out <filename");
				return EINVAL;
		}

		if ((fp = open(argv[1], O_WRONLY | O_CREAT, 777)) == -1) {
				perror(argv[1]);
				return errno;
		}

		close(fp);

		return 0;
}
