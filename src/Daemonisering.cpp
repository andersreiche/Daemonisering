//============================================================================
// Name        : Daemonisering.cpp
// Author      : Anders Reiche
// Version     : 1
// Copyright   : Open source
// Description : Daemonisering eksempel
//============================================================================

#include "Daemonisering.h"

void sig_handler(int signo);
void socket_setup(int port);
void daemonize(void);
void ping(void);

int main(int argc, char* argv[]) {

	process_id = fork(); // Create child process

	if (process_id < 0) { // Indication of fork() failure
		printf("fork failed!\n");
		exit(1);
	}

	if (process_id > 0) { // PARENT PROCESS. Need to kill it.
		printf("process_id of child process %d \n", process_id);
		exit(0);
	}

	umask(0); //unmask the file mode

	sid = setsid(); //set new session
	if (sid < 0) {
		exit(1);
	}

	socket_setup(1955); // create the TCP socket used for the listen call on the specified port
	daemonize(); // close stdin, stdout, stderr and change directory to "/"

	fp = fopen("Log.txt", "w+"); // Open a log file in write mode.

	// Calls the signal handler if SIGTERM is recieved
	if (signal(SIGTERM, sig_handler) == SIG_ERR) {
		exit(0);
	}

	listen(listen_fd, 10); //listening to the specified ip and port and have a waiting list of 10
	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL); // Accept is a blocking function. A wise man might handle this in a seperate thread

	while (1) {
		//Dont block context switches, let the process sleep for some time
		sleep(1);
		fprintf(fp, "Logging info...\n");
		fflush(fp);
		ping();
	}
	fclose(fp);
	return (0);
}
