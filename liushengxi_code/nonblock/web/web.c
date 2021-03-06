/*************************************************************************
	> File Name: web.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年08月24日 星期五 11时00分14秒
 ************************************************************************/

#include "web.h"

struct addrinfo *Host_serv(const char *host, const char *serv, int family, int socktype);
void home_pages(const char *host, const char *fname);
void start_connect(struct file *fptr); //非阻塞连接;
void write_get_cmd(struct file *fptr);

int Tcp_connect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_connect error for %s ,%s,%s : %s", host, serv, gai_strerror(n));
	ressave = res;
	do
	{
		sockfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		Close(sockfd);
	} while ((res = res->ai_next) != NULL);
	if (res == NULL)
		err_sys("tcp_coonnect error for %s,%s", host, serv);
	freeaddrinfo(ressave);
	return (sockfd);
}

struct addrinfo *Host_serv(const char *host, const char *serv, int family, int socktype)
{
	int n;
	struct addrinfo hints, *res;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("host_serv error for %s, %s: %s",
				 (host == NULL) ? "(no hostname)" : host,
				 (serv == NULL) ? "(no service name)" : serv,
				 gai_strerror(n));
	return (res);
}

void home_pages(const char *host, const char *fname)
{
	int fd, n;
	char line[MAXLINE] = {0};
	fd = Tcp_connect(host, SERV);
	n = snprintf(line, sizeof(line), GET_CMD, fname);
	Sendlen(fd, line, n, 0);
	for (;;)
	{
		if ((n = Recvlen(fd, line, MAXLINE, 0)) == 0)
			break; //serv closed
		fprintf(stderr, "recv %d bytes from server \n", n);
	}
	fprintf(stderr, "end-of-home-pages\n");
	Close(fd);
}
void start_connect(struct file *fptr) //非阻塞连接
{
	int fd, flags, n;
	struct addrinfo *ai;
	ai = Host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);
	fd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	fptr->f_fd = fd;

	fprintf(stderr, "start_connect  for %s ,fd %d \n", fptr->f_name, fd);

	flags = Fcntl(fd, F_GETFL, 0);
	Fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	if ((n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0)
	{
		if (errno != EINPROGRESS) // EINPROGRESS套接字为非阻塞套接字，且连接请求没有立即完成
			err_sys("nonblocking connect error ", __LINE__);
		fptr->f_flags = F_CONNECTING;
		FD_SET(fd, &rset);
		FD_SET(fd, &wset);
		if (fd > maxfd)
			maxfd = fd;
	}
	else if (n >= 0)
	{ /* connect is already done */
		write_get_cmd(fptr);
	}
}
void write_get_cmd(struct file *fptr)
{
	int n;
	char line[MAXLINE];
	n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);

	Sendlen(fptr->f_fd, line, n,0);

	fprintf(stderr, "send %d bytes for %s \n\n\n", n, fptr->f_name);

	fptr->f_flags = F_READING; /* clears F_CONNECTING */

	FD_SET(fptr->f_fd, &rset); /* will read server's reply */

	if (fptr->f_fd > maxfd)
		maxfd = fptr->f_fd;
}

int main(int argc, char **argv)
{
	int i, fd, n, maxconn, flags, error;
	char buf[MAXLINE] = {0};
	fd_set rs, ws;
	if (argc < 5)
	{
		fprintf(stderr, "use :web conns hostname homepages files.....");
		return 0;
	}
	maxconn = atoi(argv[1]);
	nfiles = min(argc - 4, MAXFILES);
	for (i = 0; i < nfiles; i++)
	{
		file[i].f_name = argv[i + 4];
		file[i].f_host = argv[2];
		file[i].f_flags = 0;
	}

	fprintf(stderr, "nfiles ==  %d \n", nfiles);

	home_pages(argv[2], argv[3]); //建立第一个连接

	FD_ZERO(&rset);
	FD_ZERO(&wset);

	maxfd = -1;
	nlefttoread = nlefttoconn = nfiles;
	nconn = 0;

	/*
nconn :当前打开的连接数，不超过第一个命令行参数
nlefttoread:待读取的文件数量
nlefttoconn:尚未连接的文件数
nfiles:文件数量
*/
	while (nlefttoread > 0)
	{
		while (nconn < maxconn && nlefttoconn > 0)
		{
			/* 4find a file to read */
			for (i = 0; i < nfiles; i++)
				if (file[i].f_flags == 0)
					break;
			if (i == nfiles)
				err_quit("nlefttoconn = %d but nothing found", nlefttoconn);
			start_connect(&file[i]);
			nconn++;
			nlefttoconn--;
		}

		rs = rset;
		ws = wset;
		n = Select(maxfd + 1, &rs, &ws, NULL, NULL);

		for (i = 0; i < nfiles; i++)
		{
			flags = file[i].f_flags;
			if (flags == 0 || flags & F_DONE)
				continue;
			fd = file[i].f_fd;
			if (flags & F_CONNECTING &&
				(FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws)))
			{
				n = sizeof(error);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &n) < 0 ||
					error != 0)
				{
					err_ret("nonblocking connect failed for %s",
							file[i].f_name);
					file[i].f_flags = F_DONE;
				}
				/* 4connection established */
				fprintf(stderr, "connection established for %s\n", file[i].f_name);
				FD_CLR(fd, &wset);		 /* no more writeability test */
				write_get_cmd(&file[i]); /* write() the GET command */
			}
			else if (flags & F_READING && FD_ISSET(fd, &rs))
			{
				if ((n = read(fd, buf, sizeof(buf))) == 0)
				{
					fprintf(stderr, "end-of-file on %s\n", file[i].f_name);
					Close(fd);
					file[i].f_flags = F_DONE; /* clears F_READING */
					FD_CLR(fd, &rset);
					nconn--;
					nlefttoread--;
				}
				else
				{
					fprintf(stderr, "read %d bytes from %s\n", n, file[i].f_name);
				}
			}
		}
	}
	exit(0);
}
