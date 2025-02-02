#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	// 0 args: error & exit
	if (argc <= 1) {
		errno = EINVAL;
		perror("EINVAL: (0 args)");
		exit(errno);
	}

	// discussion method: fork and pipe combo <3
	int pipe_old[2]; // 0 = read end | 1 = write end
	int pipe_new[2];

	int last = argc - 1;

	for (int i = 1; i < argc; i++) {

		// 1 arg: run the program & return
		if (argc == 2) {
			if (execlp(argv[1], argv[1], NULL) == -1) { // failed execution
				perror("Failed to run a singular program (1 arg)");
				exit(errno);
			}
			return 0; // successful
		} 

		// create a new pipe (unless not last arg)
		if (i != last)
			if (pipe(pipe_new) == -1) // failed pipe
			{
				perror("Failed to create pipe");
				exit(errno);
			}

		// FORK: create child process
		int pid = fork();

		// fork failed
		if (pid < 0) {
			perror("Failed to fork");
			exit(errno);
		}

		// child process
		if (pid == 0) {
			// read from previous pipe (if not 1st arg)
			if (i > 1) {
				dup2(pipe_old[0], STDIN_FILENO);
				close(pipe_old[0]);
			}

			// output into new pipe (if not last arg)
			if (i != last) {
				dup2(pipe_new[1], STDOUT_FILENO);
			}

			// done with new pipe
			close(pipe_new[0]);
			close(pipe_new[1]);

			// execute the arg
			if (execlp(argv[i], argv[i], NULL) == -1) {
				perror("Failed to execute arg in child");
				exit(errno);
			}
			
			return 0;
		} else { // parent process
			// wait for child & store child's exit status into 'status'
			int status;
			if (wait(&status) == -1) {
				perror("Failed to wait for child process");
				exit(errno);
			}
			if (WEXITSTATUS(status)) { // catch errors in child (exit status != 0)
				perror("Parent caught error in child process");
				return (WEXITSTATUS(status));
			}

			// store new -> old pipe, so that you can create new pipe for next arg
			if (i != last) {
				// done writing to new pipe! but don't delete it
				close(pipe_new[1]);
				pipe_old[0] = pipe_new[0];
				pipe_old[1] = pipe_new[1];
			} else {
				// last arg: we are done, close everything!
				close(pipe_old[0]);
				close(pipe_old[1]);
				close(pipe_new[0]);
				close(pipe_new[1]);
			}
		}
	}

	return 0;
}