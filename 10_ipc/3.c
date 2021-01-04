#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>

#define BUF_SIZE 5
#define N_ITEMS 10

int main(void) {

		int *buf;
		sem_t *sem_fill;
		sem_t *sem_empty;
		int pid;
		int i;

		printf("Maximum number of elements in buffer: %d\n", BUF_SIZE);
		printf("Number of items to be produced and consumed: %d\n", N_ITEMS);

		if ((buf = (int *)mmap(NULL,
						BUF_SIZE * sizeof(int),
						PROT_READ | PROT_WRITE,
						MAP_SHARED | MAP_ANONYMOUS,
						-1,
						0)) == (void *)-1) {
				perror("mmap 1");
				return errno;
		}

		if ((sem_fill = (sem_t *)mmap(NULL,
						sizeof(sem_t),
						PROT_READ | PROT_WRITE,
						MAP_SHARED | MAP_ANONYMOUS,
						-1,
						0)) == (void *)-1) {
				perror("mmap fill");
				return errno;
		}

		if (sem_init(sem_fill, 1, 0) == -1) {
				perror("init fill");
				return errno;
		}

		if ((sem_empty = (sem_t *)mmap(NULL,
						sizeof(sem_t),
						PROT_READ | PROT_WRITE,
						MAP_SHARED | MAP_ANONYMOUS,
						-1,
						0)) == (void *)-1) {
				perror("mmap 2");
				return errno;
		}

		if (sem_init(sem_empty, 1, BUF_SIZE) == -1) {
				perror("init empty");
				return errno;
		}

		if ((pid = fork()) == -1) {
				perror("fork");
				return errno;
		}
		else if (pid) {
				/* parent, producer */
				
				for (i = 0; i < N_ITEMS; i++) {
						if (sem_wait(sem_empty) == -1) {
								perror("wait in producer");
								return errno;
						}

						buf[i % BUF_SIZE] = i;
						printf("Writing %d into buffer\n", i);

						if (sem_post(sem_fill) == -1) {
								perror("post in producer");
								return errno;
						}
				}
		}
		else {
				/* child, consumer */

				for (i = 0; i < N_ITEMS; i++) {

						if (sem_wait(sem_fill) == -1) {
								perror("wait in consumer");
								return errno;
						}

						printf("Read %d from buffer\n", buf[i % BUF_SIZE]);

						if (sem_post(sem_empty) == -1) {
								perror("post in consumer");
										return errno;
						}
				}
		}

		return 0;
}

