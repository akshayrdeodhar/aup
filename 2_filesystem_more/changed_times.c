#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define TRUTH "What do you get when you multiply 9 by 6"
#define BUFSIZE 256

void print_times(struct stat *st) {
		printf("Last status change:     %s", ctime(&(st->st_ctime)));
		printf("Last file access:       %s", ctime(&(st->st_atime)));
		printf("Last file modification: %s", ctime(&(st->st_mtime)));
}



int main(void) {

		if (system("echo \"What does 42 mean?\" > file1") == -1) {
				perror("unable to execute command");
				return errno;
		}

		/* assert: file1 exists and has content */

		int fp;
		struct stat file_info;

		if (stat("file1", &file_info) == -1) {
				perror("file1");
				return errno;
		}

		printf("Time Fields before calling open():\n");
		print_times(&file_info);

		if ((fp = open("file1", O_WRONLY | O_CREAT, 777)) == -1) {
				perror("file1");
				return errno;
		}

		close(fp);

		if (stat("file1", &file_info) == -1) {
				perror("file1");
				return errno;
		}

		printf("Time fields after calling open()\n");
		print_times(&file_info);

		return 0;
}
