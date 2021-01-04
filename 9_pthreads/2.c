#define _GNU_SOURCE
#include <sched.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void *busy_void(void *arg) {
		int count = 100000;
		while (count--);
		return NULL;
}

int main(void) {
		
		int n;

		if ((n = sysconf(_SC_NPROCESSORS_CONF)) == -1) {
				perror("sysconf");
				return errno;
		}

		printf("Number of CPUs: %d\n", n);

		pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * n);
		cpu_set_t *cpus = (cpu_set_t *)malloc(sizeof(cpu_set_t) * n);

		int i;
		for (i = 0; i < n; i++) {
				CPU_ZERO(&cpus[i]);
				CPU_SET(i, &cpus[i]);
		}

		for (i = 0; i < n; i++) {
				if (pthread_create(&threads[i], 
								   NULL,
								   busy_void,
								   NULL) == -1) {
						fprintf(stderr, "Unable to create thread\n");
				}
		}

		/* DANGER: non-POSIX code */
		for (i = 0; i < n; i++) {
				if (pthread_setaffinity_np(threads[i],
										   sizeof(cpu_set_t),
										   &cpus[i]) == -1) {
						fprintf(stderr, "Unable to set affinity\n");
						return 1;
				}
		}

		for (i = 0; i < n; i++) {
				if (pthread_getaffinity_np(threads[i],
										   sizeof(cpu_set_t),
										   &cpus[i]) == -1) {
						fprintf(stderr, "Unable to get affinity\n");
						return 2;
				}
				printf("Thread %d is ", i);
				if (!CPU_ISSET(i, &cpus[i])) {
						printf("not ");
				}
				printf("attached to CPU %d\n", i);

				if (pthread_join(threads[i], NULL) == -1) {
						fprintf(stderr, "Unable to join with thread %lu\n", threads[i]);
				}

		}


		return 0;

}
