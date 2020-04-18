#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <ev.h>

#define offsetof(type, member) ((size_t) & ((type *)0)->member)
#define container_of(ptr, type, member) \
	({const typeof(((type *)0)->member) *_mptr = ptr; \
		(type *)((char *)_mptr - offsetof(type, member));})

#define MAX_BUF 1024

void FATAL(const char *msg)
{
	printf("%s\n", msg);
	exit(EXIT_FAILURE);
}

void ERROR(const char *s)
{
	char *msg = strerror(errno);
	printf("%s: %s\n", s, msg);
}

void *ss_malloc(size_t size)
{
	void *tmp = malloc(size);
	if (tmp == NULL) {
		FATAL("Cannot malloc mem.");
	}

	return tmp;
}

typedef struct server_ctx {
	ev_io io;
	int connected;
	struct server *server;
}server_ctx_t;

typedef struct server {
	int sockfd;
	char *buf;
	int len;
	int idx;
	struct server_ctx *recv_ctx;
	struct server_ctx *send_ctx;
}server_t;

typedef struct listen_ctx {
	ev_io io;
	int sockfd;
}listen_ctx_t;

static int setnonblocking(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		flags = 0;
	}

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int create_and_bind(const char *port)
{
	int listen_fd = 0;
	socklen_t addrlen = 0;
	struct sockaddr_in serv_addr;

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(port));
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	addrlen = sizeof(struct sockaddr_in);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		FATAL("create_and_bind: socket() error.");
	}

	if(bind(listen_fd, (struct sockaddr *)&serv_addr, addrlen) == -1) {
		ERROR("bind");
		close(listen_fd);
	}

	return listen_fd;
}

static void free_server(server_t *server)
{
	if (server->buf != NULL) {
		free(server->buf);
		server->buf = NULL;
	}
	if (server->recv_ctx != NULL) {
		free(server->recv_ctx);
		server->recv_ctx = NULL;
	}
	if (server->send_ctx != NULL) {
		free(server->send_ctx);
		server->send_ctx = NULL;
	}
	if (server != NULL) {
		free(server);
		server = NULL;
	}
}

static void close_and_free_server(EV_P_ server_t *server)
{
	if (server != NULL) {
		ev_io_stop(EV_A_ & server->send_ctx->io);
		ev_io_stop(EV_A_ & server->recv_ctx->io);
		close(server->sockfd);
		free_server(server);
	}
}

static void server_recv_cb(EV_P_ ev_io *w, int revents)
{
	/*
	   struct server_cxt *server_recv_ctx = container_of(w, server_ctx_t, io);
	   server_t *server = container_of(server_recv_ctx, struct server, recv_ctx);
	   */
	server_ctx_t *server_recv_ctx = (server_ctx_t *)w;
	server_t *server              = server_recv_ctx->server;

	ssize_t r = recv(server->sockfd, server->buf, MAX_BUF, 0);
	if (r == 0) {
		close_and_free_server(EV_A_ server);
		return;
	} else if (r == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return;
		} else {
			ERROR("server_recv_cb: server recv.");
			close_and_free_server(EV_A_ server);
			return;
		}
	} else {
		printf("recv: %s\n", server->buf);
	}

	server->len = r;
}

static void server_send_cb(EV_P_ ev_io *w, int revents)
{
	/*
	   server_ctx_t *server_recv_ctx = container_of(w, server_ctx_t, io);
	   server_t *server = container_of(server_recv_ctx, server_t, recv_ctx);
	   */
	server_ctx_t *server_send_ctx = (server_ctx_t *)w;
	server_t *server              = server_send_ctx->server;

	ssize_t s = send(server->sockfd, server->buf + server->idx, server->len, 0);
	if (s == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			ERROR("server_send_cb: server send.");
			close_and_free_server(EV_A_ server);
		}
		return;
	} else if (s < MAX_BUF) {
		server->len -= s;
		server->idx = s;
		return;
	} else {
		memset(server->buf, 0, MAX_BUF);
		return;
	}
}

static server_t *new_server(int fd)
{
	server_t *server = ss_malloc(sizeof(server_t));
	memset(server, 0, sizeof(server_t));

	server->recv_ctx = ss_malloc(sizeof(server_ctx_t));
	server->send_ctx = ss_malloc(sizeof(server_ctx_t));
	server->buf = ss_malloc(MAX_BUF);
	memset(server->recv_ctx, 0, sizeof(server_ctx_t));
	memset(server->send_ctx, 0, sizeof(server_ctx_t));
	server->sockfd = fd;
	server->recv_ctx->server = server;
	server->send_ctx->server = server;

	ev_io_init(&server->recv_ctx->io, server_recv_cb, fd, EV_READ);
	ev_io_init(&server->send_ctx->io, server_send_cb, fd, EV_WRITE);

	return server;
}

static void accept_cb(EV_P_ ev_io *w, int revents)
{
	struct listen_ctx *listener = container_of(w, struct listen_ctx, io);
	int server_fd = accept(listener->sockfd, NULL, NULL);
	if (server_fd == -1) {
		ERROR("accept");
		return;
	}

	setnonblocking(server_fd);

	server_t *server = new_server(server_fd);

	ev_io_start(EV_A_ &server->recv_ctx->io);
	ev_io_start(EV_A_ &server->send_ctx->io);
}

int main(int argc, char **argv)
{
	int ret = 0;
	int listen_fd = 0;

	struct ev_loop *loop = EV_DEFAULT;

	struct listen_ctx listen_ctx;
	memset(&listen_ctx, 0, sizeof(struct listen_ctx));

	if (argc != 2) {
		FATAL("server: No specify the port.");
	}

	listen_fd = create_and_bind(argv[1]);
	if (listen_fd == -1) {
		FATAL("server: bind() error.");
	}
	if (listen(listen_fd, SOMAXCONN) == -1) {
		FATAL("server: listen() error.");
	}

	setnonblocking(listen_fd);

	listen_ctx.sockfd = listen_fd;

	ev_io_init(&listen_ctx.io, accept_cb, listen_fd, EV_READ);
	ev_io_start(loop, &listen_ctx.io);

	ev_run(loop, 0);
	return ret;
}
