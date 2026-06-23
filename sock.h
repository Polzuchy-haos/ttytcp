#ifndef SOCK_H_
#define SOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sock_t sock_t;

sock_t* sock_open(const char* ip, int port);
void    sock_close(sock_t* sock);
sock_t* sock_assert(sock_t* sock);

int     sock_read(sock_t* sock, char* buf, int len);
int     sock_write(sock_t* sock, char* buf, int len);

#ifdef __cplusplus
}
#endif

#endif //SOCK_H_