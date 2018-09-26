/*************************************************************************
	> File Name: web01.c
	> Author: Liu Shengxi 
	> Mail: 13689209566@163.com
	> Created Time: 2018年09月26日 星期三 20时15分45秒
 ************************************************************************/

#include "/home/Shengxi-Liu/unix_network_progream_code/liushengxi_code/myhead.h"

#define MAXFILES 20
#define SERV "80"
struct file
{
	char *f_name;
	char *f_host;
	int f_fd;
	int f_flags;
	pthread_t f_tid;
} file[MAXFILES];

#define F_CONNECTING 1
#define F_READING 2
#define F_DONE 4

#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
/*
nconn:当前打开的连接数，不超过第一个命令行参数
nlefttoread:待读取的文件数量
nlefttoconn:尚未连接的文件数
nfiles:文件数量
*/
void do_get_read(void *);
void home_pages(const char *, const char *);
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
void write_get_cmd(struct file *fptr)
{
	int n;
	char line[MAXLINE];
	n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);

	Sendlen(fptr->f_fd, line, n, 0);

	fprintf(stderr, "send %d bytes for %s \n\n\n", n, fptr->f_name);

	fptr->f_flags = F_READING; /* clears F_CONNECTING */
}
void *do_get_read(void *vptr)
{
	int fd, n;
	char line[MAXLINE];
	struct file *fptr;
	fd = Tcp_connect(fptr->f_host, SERV); // 与服务器进行连接
	fptr->f_fd = fd;
	printf("do_get_read for %s,fd %d ,thread %d \n", fptr->f_name, fd, fptr->f_tid);
	write_get_cmd(fptr);// 发出 get 请求
	for (;;)
	{
		if ((n = recv(fd, line, MAXLINE, 0)) == 0)
			printf("对端关闭！！！\n"), break;
		printf("recv %d bytes form %s \n", n, fptr->f_name);
	}
	printf("end-of \n");
	Close(fd);
	fptr->f_flags = F_DONE;
	return (fptr);
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
int main(int argc, char **argv)
{
	int i, n, maxconn;
	pthread_t tid;
	struct file *fptr;
	if (argc < 5)
	{
		err_quit(" use :web conns hostname homepages files.....")
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
	nlefttoread = nlefttoconn = nfiles;
	nconn = 0;
	while (nlefttoread > 0)
	{
		while (nconn < maxconn && nlefttoconn > 0)
		{
			for (i = 0; i < nfiles; i++)
			{
				if (file[i].f_flags == 0)
					break;
				if (i == nfiles)
					err_quit("nlefttoconn ==  %d but nothing found", nlefttoconn);
				file[i].f_flags = F_CONNECTING;
				pthread_create(&tid, NULL, &do_get_read, &file[i]);
				file[i].f_tid = tid;
				nconn++;
				nlefttoconn--;
			}
		}
		if ((n = thr_join(0, &tid, (void **)&fptr)) != 0) //只有 Solaris 系统中才有（等待任一线程结束）
		{
			errno = n;
			err_sys("thr_join error ");
		}
		nconn--;
		nlefttoread--;
		printf("thread id %d for %s done \n", tid, fptr->f_name);
	}
	exit(0);
}