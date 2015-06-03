#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>

#include "socket.h"
int wrap_client_ip(const struct in_addr *addr, int port)
{
	int sock=socket(AF_INET, SOCK_STREAM, 0);
	int tmperr;
	if (sock<0)
	{
		tmperr=errno;//temporarial int for saving errno
		perror("socket creation error");
		errno=tmperr;
		return sock;
	}

	struct sockaddr_in to;
	bzero(&to, sizeof(struct sockaddr_in));
	to.sin_family=AF_INET;
	to.sin_port=htons(port);
	to.sin_addr=*addr;

	if (connect(sock, (SA *)&to, sizeof(to))<0)
	{
		tmperr=errno;
		close(socket);
		errno=tmperr;
		return -1;
	}
	return sock;
}
int wrap_client_str(const char *ip, int port)
{
	struct in_addr inp;
	int tmperr;
	if (inet_aton(ip, &inp)==0){
		tmperr=errno;
		perror("inet_aton error");
		errno=tmperr;
		return -1;
	}
	return wrap_client_ip(&inp, port);
}

int wrap_client(const char *ip_port)
{
	char *col, *c;
	char buf[100];
	int port;
	if (ip_port==NULL)
		return -1;
	bcopy(ip_port, buf, strlen(ip_port));
	for (c=buf; *c!=':'; c++);
	*c='\0';
	c++;
	port=atoi(c);
	return wrap_client_str(buf, port);
}

ssize_t wrap_send(int sock, const void *buf, size_t len, int flags)
{
	ssize_t sent=0, res, ret;
	int tmperr;

	while (sent<len){
		res=send(sock, buf+sent, len-sent, flags);
		if (res<0){
			tmperr=errno;
			perror("wrap_send");
			if (tmperr==EINTR || tmperr==EAGAIN || tmperr==EWOULDBLOCK)
				continue;
			errno=tmperr;
			return -1;
		}
		sent+=res;
	}
	return sent;
}

ssize_t wrap_recv(int sock, void *buf, size_t len, int flags)
{
	ssize_t recvd=0, res;
	int tmperr;	
	int timeout_cnt = 0;
	while (recvd<len){
		res=recv(sock, buf+recvd, len-recvd, flags);
		if (res<0){
			tmperr=errno;
			if (tmperr==EINTR || tmperr==EAGAIN || tmperr==EWOULDBLOCK)
				continue;
			errno=tmperr;
			return res;
		}
		else if(res == 0)
		{
			usleep(10000);
			printf("TimeoutCnt=%d\n",timeout_cnt);
			if(timeout_cnt++ >= SOCKET_TIMEOUT_CNT)
			{
				printf("recv timeout\n");
				return 0;
			}
		}
		recvd+=res;
	}
	return recvd;
}
void mksock_send_ip(char *argv1,char *argv2)
{
	char a[5];
	int i,j;
	for (i = 0; i < 4; i ++)
	{
		a[i] = argv2[i];
	}
	a[4] = argv2[4];
	for (i = 0; i < strlen(argv1) - 4; i++)
	{
		argv2[i] = argv1[i];
	}
	for (j=0;j<4; i++,j++)
	{
		argv2[i] = a[j];
	}
	argv2[i] = '\0';
	
}

