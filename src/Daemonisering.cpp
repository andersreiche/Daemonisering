//============================================================================
// Name        : Daemonisering.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>

void sig_handler(int signo);

int main(int argc, char* argv[]) {
	FILE *fp = NULL;
	pid_t process_id = 0;
	pid_t sid = 0;
// Create child process
	process_id = fork();
// Indication of fork() failure
	if (process_id < 0) {
		printf("fork failed!\n");
// Return failure in exit status
		exit(1);
	}
// PARENT PROCESS. Need to kill it.
	if (process_id > 0) {
		printf("process_id of child process %d \n", process_id);
// return success in exit status
		exit(0);
	}
//unmask the file mode
	umask(0);
//set new session
	sid = setsid();
	if (sid < 0) {
// Return failure
		exit(1);
	}
// Change the current working directory to root.
	chdir("/");
// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
// Open a log file in write mode.
	fp = fopen("Log.txt", "w+");

	if (signal(SIGTERM, sig_handler) == SIG_ERR) {
				exit(0);
			}

	while (1) {
//Dont block context switches, let the process sleep for some time
		sleep(1);
		fprintf(fp, "Logging info...\n");
		fflush(fp);


// Implement and call some function that does core work for this daemon.
	}
	fclose(fp);
	return (0);
}

void sig_handler(int signo) {
	openlog("Daemonisering ", LOG_PID, LOG_USER);
	if (signo == SIGTERM)
		syslog(LOG_INFO, "SIGTERM recieved, exiting");
		closelog();
		exit(0);
}
