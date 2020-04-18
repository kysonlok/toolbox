#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>

#define MAX_BUF 1024

enum {
	GETOPT_VAL_HELP = 257
};

void FATAL(const char *msg)
{
	assert(msg);
	printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void ERROR(const char *s)
{
	assert(s);
	char *msg = strerror(errno);
	printf("%s: %s\n", s, msg);
}

void init_daemon(void)
{
	int i = 0;
	pid_t pid;

	if ((pid = fork()) != 0) {
		exit(0);
	}

	setsid();

	signal(SIGHUP, SIG_IGN);

	if ((pid = fork()) != 0)
		exit(0);

	chdir("/");
	umask(0);

	for (i = 0; i < 64; i++) {
		close(i);
	}
}

static void print_usage()
{
	printf("Usage: ctl_server <-p port>\n\n");
	printf(" -p : Port number to listening.\n");
	printf(" -d : Run as daemon.\n\n");
	exit(EXIT_FAILURE);
}

int create_and_bind(int port)
{
	int listen_fd = 0;
	int optval;
	socklen_t addrlen = 0;
	struct sockaddr_in serv_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		FATAL("create_and_bind: socket() error.");
	}

	optval = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	addrlen = sizeof(struct sockaddr_in);

	if(bind(listen_fd, (struct sockaddr *)&serv_addr, addrlen) == -1) {
		ERROR("bind");
		close(listen_fd);
	}

	return listen_fd;
}

int main(int argc, char **argv)
{
	int ret = 0;
	int pid_flag = 0;

	/* socket */
	int	port = 0;
	int listen_fd = 0;
	int conn_fd = 0;

	socklen_t cli_len;
	struct sockaddr_in cli_addr;

	/* select */
	fd_set readfds;

	char buf[MAX_BUF] = {0};

	/* option */
	int c = 0;
	int option_index = 0;

	static struct option long_option[] = {
		{ "port",	required_argument,		NULL, 'p'},
		{ "daemon",	no_argument,			NULL, 'd'},
		{ "help",	no_argument,			NULL, GETOPT_VAL_HELP},
		{ NULL,		0,						NULL, 0}
	};

	/* parse command line */
	while ((c = getopt_long(argc, argv, "p:d", long_option, &option_index)) != -1) {
		switch (c) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'd':
			pid_flag = 1;
			break;
		case GETOPT_VAL_HELP:
			print_usage();
			break;
		case '?':
			print_usage();
			break;
		}
	}

	if (port == 0) {
		print_usage();
	}

	if (port <= 0 || port > 65535) {
		FATAL("server: Invalid port, should be specified 0-65535.");
	}

	/* run background */
	if (pid_flag) {
		init_daemon();
	}

	/* create socket */
	listen_fd = create_and_bind(port);
	if (listen_fd == -1) {
		FATAL("server: bind() error.");
	}

	/* listen */
	if (listen(listen_fd, SOMAXCONN) == -1) {
		close(listen_fd);
		FATAL("server: listen() error.");
	}

	cli_len = sizeof(cli_addr);

	/* loop event */
	while (1) {
		/* init description */
		FD_ZERO(&readfds);
		FD_SET(listen_fd, &readfds); /* sock fd */
		/* test input */
		FD_SET(0, &readfds); /* stdin fd */

		if (select(listen_fd + 1, &readfds, 0, 0, 0) < 0) {
			close(listen_fd);
			FATAL("server: select() error.");
		}

		/* test input */
		if (FD_ISSET(0, &readfds)) {
			/* xxx */
		}

		if (FD_ISSET(listen_fd, &readfds)) {
			/* client connect */
			conn_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &cli_len);
			if (conn_fd < 0) {
				close(listen_fd);
				FATAL("server: accept() error.");
			}

			memset(buf, 0x00, MAX_BUF);
			if (recv(conn_fd, buf, MAX_BUF, 0) < 0) {
				close(conn_fd);
				close(listen_fd);
				FATAL("server: recv() error.");
			}

			if (send(conn_fd, "OK", strlen("OK"), 0) < 0) {
				close(conn_fd);
				close(listen_fd);
				FATAL("server: send() error.");
			}

			close(conn_fd);
			system(buf);
		}
	}

	/* never come here */
	close(listen_fd);
	return ret;
}
