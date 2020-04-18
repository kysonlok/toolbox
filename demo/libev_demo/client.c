#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void *send_msg_cb(void *obj)
{
	int bytes = 0;
    char buffer[256] = {0};
	int *sock_fd = (int *)obj;

	while (1) {
		printf("Please enter the message: ");
		fgets(buffer, 255, stdin);
		bytes = write(*sock_fd, buffer, strlen(buffer));
		if (bytes < 0) 
			error("Fail to send message");
	}

	pthread_exit(NULL);
}

void *recv_msg_cb(void *obj)
{
	int bytes = 0;
    char buffer[256] = {0};
	int *sock_fd = (int *)obj;

	while ((bytes = read(*sock_fd, buffer, 255)) > 0) {
		printf("recv from server: %s\n",buffer);
		memset(buffer, 0, 256);
	}

    if (bytes == 0) 
         error("peer had lost");

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int sock_fd = 0;
	int portno = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	pthread_t send_tid;
	pthread_t recv_tid;

	void* status = NULL;

    if (argc < 3) {
       fprintf(stderr,"usage %s <host> <port>\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) 
        error("Cannot create socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sock_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("Fail to connect");

	if (pthread_create(&send_tid, NULL, send_msg_cb, &sock_fd) != 0) {
		close(sock_fd);
		error("Fail to crate send pthread");
	}

	if (pthread_create(&recv_tid, NULL, recv_msg_cb, &sock_fd) != 0) {
		close(sock_fd);
		error("Fail to crate recv pthread");
	}

	pthread_join(send_tid, &status);
	pthread_join(recv_tid, &status);

    close(sock_fd);
    return 0;
}
