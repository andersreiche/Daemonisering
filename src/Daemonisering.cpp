//============================================================================
// Name        : Daemonisering.cpp
// Author      : Anders Reiche
// Version     : 1
// Copyright   : Open source
// Description : Daemonisering eksempel
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>

void sig_handler(int signo);

int main(int argc, char* argv[]) {
	char str[100];
	int listen_fd;
	int comm_fd;
	int n = 0;
	FILE *fp = NULL;
	pid_t process_id = 0;
	pid_t sid = 0;
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

	struct sockaddr_in servaddr;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP socket
	bzero(&servaddr, sizeof(servaddr)); //alternative to memset but only can set ZERO

	/* Initialize socket structure */
	servaddr.sin_family = AF_INET; //Ip V4
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //accept any of the interfaces
	servaddr.sin_port = htons(1955); //Port number

	if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) { // testing if the bind goes well
		perror("ERROR on binding");
		exit(1);
	}

	chdir("/"); // Change the current working directory to root.
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen("Log.txt", "w+"); // Open a log file in write mode.

	if (signal(SIGTERM, sig_handler) == SIG_ERR) {
		exit(0);
	}

	listen(listen_fd, 10); //listening to the specified ip and port and have a waiting list of 10
	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL); // Accept blocks

	while (1) {
		//Dont block context switches, let the process sleep for some time
		sleep(1);
		fprintf(fp, "Logging info...\n");
		fflush(fp);
		bzero(str, 100); //sets str to ZERO
		n = read(comm_fd, str, 100); //reads from the client to str and puts number of signs in to n
		if (str != 0) {
			write(comm_fd, str, n); //writes back to the client whit the convertet text
		}
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

