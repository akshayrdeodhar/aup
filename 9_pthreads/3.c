
#define _GNU_SOURCE
#include <sched.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


void *busy_void(void *arg) {
		printf("This is thread %d\n", *((int *)arg));
		return NULL;
}

#define N_THREADS 5
static int thread_ids[N_THREADS];
pthread_t threads[N_THREADS];
int main(void) {
		
		int n, i;

		n = N_THREADS;

		for (i = 0; i < n; i++) {
				thread_ids[i] = i;
				if (pthread_create(&threads[i], 
								   NULL,
								   busy_void,
								   &thread_ids[i]) == -1) {
						fprintf(stderr, "Unable to create thread\n");
				}
		}
		for (i = 0; i < n; i++) {
				if (pthread_join(threads[i], NULL) == -1) {
						fprintf(stderr, "Unable to join with thread %lu\n", threads[i]);
				}
		}


		return 0;

}
