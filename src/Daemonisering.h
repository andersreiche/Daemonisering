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

char str[100];
int listen_fd;
int comm_fd;
int n = 0;
FILE *fp = NULL;
pid_t process_id = 0;
pid_t sid = 0;

void sig_handler(int signo) {
	openlog("Daemonisering ", LOG_PID, LOG_USER);
	if (signo == SIGTERM)
		syslog(LOG_INFO, "SIGTERM recieved, exiting");
	closelog();
	exit(0);
}

void socket_setup(int port) {
	struct sockaddr_in servaddr;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP socket
	bzero(&servaddr, sizeof(servaddr)); //alternative to memset but only can set ZERO

	/* Initialize socket structure */
	servaddr.sin_family = AF_INET; //Ip V4
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //accept any of the interfaces
	servaddr.sin_port = htons(port); //Port number

	if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) { // testing if the bind goes well
		perror("ERROR on binding");
		exit(1);
	}
}

void daemonize(void) {
	chdir("/"); // Change the current working directory to root.
	// Close stdin. stdout and stderr, daemonizing the process
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

void ping(void) {
	bzero(str, 100); //sets str to ZERO
	n = read(comm_fd, str, 100); //reads from the client to str and puts number of signs in to n
	if (str != 0) {
		write(comm_fd, str, n); //writes back to the client whit the convertet text
	}
}
