#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <getopt.h>

#define CMD_PATH "/usr/bin/control_power"

#define BUF_LEN 128

enum {
	GETOPT_VAL_HELP = 257
};

void error(const char *msg)
{
    perror(msg);
    exit(0);
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
	printf("Usage: ctl_client <-h host> <-p port> <-n num>\n\n");
	printf(" -h : Host name or IP address of your server.\n");
	printf(" -p : Port number of your server.\n");
	printf(" -n : Number of client to turn on.\n\n");
	printf(" -d : Run as daemon.\n\n");
	exit(EXIT_FAILURE);
}

#if 0
int read_file(const char *pathname, char *buf, int len)
{
	int fd = 0;
	int nbytes = 0;
	
	fd = open(pathname, O_RDONLY);
	if (fd < 0) {
        fprintf(stderr,"ERROR, cannot open file: [%s]\n", pathname);
		return -1;
	}

	nbytes = read(fd, buf, len);
	if (nbytes < 0) {
		fprintf(stderr, "ERROR, cannot read file: [%s]\n", pathname);
		close(fd);
		return -1;
	}

	close(fd);
	return nbytes;
}
#endif

int check_opt(char *host, int port, int no)
{
	assert(host);

	if (port < 0 || port > 65535) {
		fprintf(stderr, "ERROR, Invalid port.\n");
		return -1;
	}

	/* client.NO only support 1-28 */
	if (no <= 0 || no > 28) {
		fprintf(stderr, "ERROR, Invalid client NO.\n");
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int pid_flag = 0;

	/* sock */
    int					sock_fd = 0;
    struct hostent		*server;
    struct sockaddr_in	serv_addr;

	/* client */
	int		port = 0;
	char	*host = NULL;
	int		clientno= 0;

	char cmd[BUF_LEN] = {0};
	char message[256] = {0};

	/* option */
	int c = 0;
	int option_index = 0;

	static struct option long_option[] = {
		{ "host",	required_argument,		NULL, 'h'},
		{ "port",	required_argument,		NULL, 'p'},
		{ "daemon",	no_argument,			NULL, 'd'},
		{ "help",	no_argument,			NULL, GETOPT_VAL_HELP},
		{ NULL,		0,						NULL, 0}
	};

	/* parse command line */
	while ((c = getopt_long(argc, argv, "h:p:n:d", long_option, &option_index)) != -1) {
		switch (c) {
		case 'h':
			host = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'n':
			clientno = atoi(optarg);
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

	/* check option */
	ret = check_opt(host, port, clientno);
	if (ret < 0) {
		/* fprintf(stderr, "ERROR, Invalid option argument.\n"); */
		exit(EXIT_FAILURE);
	}

	/* run background */
	if (pid_flag) {
		init_daemon();
	}

	/* create socket */
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
        error("Cannot create socket");

	/* get ip address */
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	/* init protocol */
	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);

	/* connect to server */
    if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("Fail to connect");

	fprintf(stderr, "INFO, success connect to server!\n");

	sprintf(cmd, "%s %d", CMD_PATH, clientno);
	
	/* send command to server */
	if (send(sock_fd, cmd, strlen(cmd) + 1, 0) < 0) {
		fprintf(stderr, "ERROR, Failed to send message to server.\n");
		goto error;
	}

	/* receive result from server */
	memset(message, 0x00, 256);
	if (recv(sock_fd, message, 256, 0) < 0) {
		fprintf(stderr, "ERROR, Failed to receive message from server.\n");
		goto error;
	}

	puts(message);

error:
    close(sock_fd);
    return 0;
}
