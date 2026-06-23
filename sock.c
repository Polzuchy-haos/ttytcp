#include "sock.h"
//#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if defined _WIN32
// Win32 platform


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

struct sock_t{
  SOCKET fd;
  struct sockaddr_in adr;
};

static struct sock_list_t{
  sock_t s;
  struct sock_list_t* last;
  struct sock_list_t* next;
};

WSADATA wsa;
#elif defined __linux__

#else
  #error "Unsupported OS"
#endif

struct sock_list_t* head = NULL;
struct sock_list_t* tail = NULL;

static sock_t* _pf_open(const char* ip, int port, int backlog);
static void    _pf_close(sock_t* sock);
static sock_t* _pf_assert(sock_t* sock);

static int     _pf_read(sock_t* sock, char* buf, int len);
static int     _pf_write(sock_t* sock, char* buf, int len);


sock_t* sock_open(const char* ip, int port, int backlog){
  return _pf_open(ip, port, backlog);
}

void sock_close(sock_t* sock){
  _pf_close(sock);
}

sock_t* sock_assert(sock_t* sock){
  return _pf_assert(sock);
}

int sock_read(sock_t* sock, char* buf, int len){
  return _pf_read(sock, buf, len);
}

int sock_write(sock_t* sock, char* buf, int len){
  return _pf_write(sock, buf, len);
}

#if defined _WIN32

static sock_t* _pf_open(const char* ip, int port, int backlog){
  if(head == NULL && tail == NULL){
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
      perror("Error WSAStartup");
      exit(1);
    }

    head = malloc(sizeof(struct sock_list_t));
    memset(head, 0, sizeof(struct sock_list_t));
    tail = head;
  }
  else{
    tail->next = malloc(sizeof(struct sock_list_t));
    memset(tail->next, 0, sizeof(struct sock_list_t));
    tail->next->last = tail;
    tail = tail->next;
  }

  if ((tail->s.fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    perror("Error creat socket");
    tail = tail->last;
    free(tail);    
    return NULL;
  }

  tail->s.adr.sin_family = AF_INET;
  tail->s.adr.sin_addr.s_addr = INADDR_ANY;
  tail->s.adr.sin_addr.S_un.S_addr = INADDR_ANY;
  tail->s.adr.sin_port = htons(port);

  if (bind(tail->s.fd, (struct sockaddr *)&tail->s.adr, sizeof(tail->s.adr)) == SOCKET_ERROR) {
    perror("Error bind socket");
    closesocket(tail->s.fd);
    tail = tail->last;
    free(tail);
    return NULL;
  }

  listen(tail->s.fd, backlog);

  return tail;
}

static void _pf_close(sock_t* sock){
  struct sock_list_t* this = (struct sock_list_t*)sock;

  if(this->last == head)
    head = head->next;
  else if(this == tail)
    tail = tail->last;
  else
    this->last->next = this->next;
  
  closesocket(this->s.fd);
  free(this);


}

static sock_t* _pf_assert(sock_t* sock){
  tail->next = malloc(sizeof(struct sock_list_t));
  memset(tail->next, 0, sizeof(struct sock_list_t));
  tail->next->last = tail;
  tail = tail->next;

  int client_addr_len;
  tail->s.fd = accept(sock->fd, (struct sockaddr *)&tail->s.adr, &client_addr_len);
  if(tail->s.fd != INVALID_SOCKET) return &tail->s;
  
  perror("Error assert");
  return NULL;
}

static int _pf_read(sock_t* sock, char* buf, int len){
  return recv(sock->fd, buf, len, 0);
}

static int _pf_write(sock_t* sock, char* buf, int len){
  return send(sock->fd, buf, len, 0);
}

#elif defined __linux__

#else
  #error "Unsupported OS"
#endif