#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#define SOCKET_TIMEOUT_CNT    50  // if can't connect for more than 50 times, the copter will land
typedef struct sockaddr SA;
int wrap_client_ip(const struct in_addr *addr, int port);
int wrap_client_str(const char *ip, int port);
int wrap_client(const char *ip_port);
ssize_t wrap_send(int sock, const void *buf, size_t len, int flags);
ssize_t wrap_recv(int sock, void *buf, size_t len, int flags);
void mksock_send_ip(char *argv1,char *argv2);




#endif  
