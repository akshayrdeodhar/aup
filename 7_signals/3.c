#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SHARED_ARR "shared_array"
#define SEMAPHORE_FORKING "fork_semaphore"
#define N 6

/* to ensure that fp can be used by all processes */
static int fp;

void print_message(int signo) {
		printf("%d got hit by signal %d\n", getpid(), signo);
}

void examine_child(int pid) {
	int status;

	if (waitpid(pid, &status, 0) == -1) {
			perror("wait");
			exit(errno);
	}

	if (WIFEXITED(status)) {
			printf("%d exited with %d\n", pid, WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status)) {
			printf("%d killed by signal %d\n", pid, WTERMSIG(status));
	}
	else {
			printf("%d died in some other way\n", pid);
	}
}

void *get_shared_arr() {
		void *buf;
		/* asssert: fp is a file descriptor which points to the shared memory,
		 * and is inherited by all processes */
		if ((buf = mmap(NULL, sizeof(int) * N, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0)) == MAP_FAILED) {
				perror("shared memory mmap() failed");
				exit(errno);
		}
		return buf;
}


int main(void) {

		/* process 0 */
		/* can see the pids of {1, 2} */
		sem_t *fork_sem, *pgid_sem, *exit_sem;
		int *child_pid;
		int ret;

		if (signal(SIGUSR1, print_message) == SIG_ERR) {
				perror("SIGUSR1");
				return errno;
		}

		/* link shared memory address to process- this will be visible in
		 * children */
		if ((child_pid= mmap(NULL, sizeof(int) * N, 
										PROT_READ | PROT_WRITE, 
										MAP_SHARED | MAP_ANONYMOUS, 
										-1, 0)) == MAP_FAILED) {

				perror("mmap");
				return errno;
		}

		/* create a shared memory map for semaphore */
		if ((fork_sem = mmap(NULL, sizeof(sem_t), 
										PROT_READ | PROT_WRITE, 
										MAP_SHARED | MAP_ANONYMOUS, 
										-1, 0)) == MAP_FAILED) {

				perror("semaphore");
				return errno;
		}
		/* initialize semaphore with initial value 0- when 5 and 3 get
		 * created, this is incremented by 1 each. 0 will synchronize by calling
		 * down() on this twice */
		if (sem_init(fork_sem, 1, 0) == -1) {
				perror("semaphore initialization");
				return errno;
		}

		/* create a shared memory map for semaphore */
		if ((exit_sem = mmap(NULL, sizeof(sem_t), 
										PROT_READ | PROT_WRITE, 
										MAP_SHARED | MAP_ANONYMOUS, 
										-1, 0)) == MAP_FAILED) {

				perror("semaphore");
				return errno;
		}

		/* initialize semaphore with initial value 0- when signals have been
		 * sent, this will be incremented 5 times, wherin all processes will
		 * exit */
		if (sem_init(exit_sem, 1, 0) == -1) {
				perror("semaphore initialization");
				return errno;
		}

		/* create a shared memory map for semaphore */
		if ((pgid_sem = mmap(NULL, sizeof(sem_t), 
										PROT_READ | PROT_WRITE, 
										MAP_SHARED | MAP_ANONYMOUS, 
										-1, 0)) == MAP_FAILED) {

				perror("semaphore");
				return errno;
		}

		/* initialize semaphore with initial value 0- when all processes have
		 * called setpgid, the parent can send signals */
		if (sem_init(pgid_sem, 1, 0) == -1) {
				perror("semaphore initialization");
				return errno;
		}

		/* assert: now each child will have access to the semaphore, unless the
		 * memory region is purposely unliked */


		if ((ret = fork()) == -1) {
				perror("fork 1");
				return errno;
		}
		else if (!ret) {
				/* child 1 */
				/* can see pids of {5} */
				/* child_pid = (int *)get_shared_arr(); */

				if ((ret = fork()) == -1) {
						perror("fork 5");
						return errno;
				}
				else if (!ret) {
						/* chlid 5 */
						/* can see pids of {} */
						/* child_pid = (int *)get_shared_arr(); */

						/* wait for the PID of 3 to be available */
						if (sem_wait(fork_sem) == -1) {
								perror("P operation in 5");
						}

						/* set own process group to 3 */
						if (setpgid(0, child_pid[3]) == -1) {
								perror("setpgid(5, 3)");
								return errno;
						}
				
						if (sem_post(pgid_sem) == -1) {
								perror("setpgid(5, 3) done synchronization");
								return errno;
						}


						/* wait for 0 to allow exiting */
						if (sem_wait(exit_sem) == -1) {
								perror("V operation in 4");
								return errno;
						}

						printf("%d is child of %d\n", getpid(), getppid());

						return 0;
				}
				child_pid[5] = ret;



				/* wait for 0 to allow exiting */
				if (sem_wait(exit_sem) == -1) {
						perror("V operation in 2");
						return errno;
				}

				printf("%d is child of %d\n", getpid(), getppid());

				examine_child(child_pid[5]);

				return 0;
		}
		child_pid[1] = ret;

		if ((ret = fork()) == -1) {
				perror("fork 2");
				return errno;
		}
		else if (!ret) {
				/* child 2 */
				/* can see pids of {3} */
				/* child_pid = (int *)get_shared_arr(); */

				if ((ret = fork()) == -1) {
						perror("fork 3");
						return errno;
				}
				else if (!ret) {
						/* child 3*/
						/* can see pids of {4} */

						child_pid[3] = getpid();

						if (setpgid(child_pid[3], child_pid[3]) == -1) {
								perror("setpgid(3, 3");
								return errno;
						}

						if (sem_post(pgid_sem) == -1) {
								perror("setpgid(3, 3) done synchronization");
								return errno;
						}

						/* tell 5  that pid of 3 is available in shared memory,
						 * and it can call setpgid safely*/
						if (sem_post(fork_sem) == -1) {
								perror("P operation in 3");
								return errno;
						}


						if ((ret = fork()) == -1) {
								perror("fork 4");
								return errno;
						}
						else if (!ret) {
								/* child 4*/
								/* can see pids of {} */
								/* child_pid = (int *)get_shared_arr(); */

								if (setpgid(0, child_pid[3]) == -1) {
										perror("setpgid(4, 3)");
										return errno;
								}

								/* tell 0 that 4 has moved to new process group
								 * */
								if (sem_post(pgid_sem) == -1) {
										perror("setpgid(4, 3) done synchronization");
										return errno;
								}


								/* wait for 0 to allow exiting */
								if (sem_wait(exit_sem) == -1) {
										perror("V operation in 4");
										return errno;
								}

								printf("%d is child of %d\n", getpid(), getppid());

								return 0;

						}

						child_pid[4] = ret;


						if (sem_wait(exit_sem) == -1) {
								perror("V operation in 3");
								return errno;
						}

						printf("%d is child of %d\n", getpid(), getppid());

						examine_child(child_pid[4]);

						return 0;
				}
				child_pid[3] = ret;


				/* wait for 0 to allow exiting */
				if (sem_wait(exit_sem) == -1) {
						perror("V operation in 2");
						return errno;
				}

				printf("%d is child of %d\n", getpid(), getppid());

				examine_child(child_pid[3]);

				return 0;
		}
		child_pid[2] = ret;

		/* wait till all processes created, and the 3 setpgid calls finish */
		int i;
		for (i = 0; i < 3; i++) {
				if (sem_wait(pgid_sem) == -1) {
						perror("V operation in 0");
						return errno;
				}
		}

		child_pid[0] = getpid();

		if (kill(-child_pid[3], SIGUSR1) == -1) {
				perror("kill");
				return errno;
		}

		for (i = 0; i <= 5; i++) {
				printf("Process %d = %d\n", i, child_pid[i]);
		}


		for (i = 0; i < 5; i++) {
				if (sem_post(exit_sem) == -1) {
						perror("P by 0");
						return errno;
				}
		}

		examine_child(child_pid[1]);
		examine_child(child_pid[2]);

		return 0;

}
