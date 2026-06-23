#include "tty.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if defined _WIN32
// Win32 platform

#include <windows.h>

struct tty_t{
  char path[32];
  HANDLE fd;
  DCB cfg;
  COMMTIMEOUTS timeout;
};

#elif defined __linux__

#else
  #error "Unsupported OS"
#endif



//-----------------------------------------------------------------------------
//--------------------- Private prototype function ----------------------------
//-----------------------------------------------------------------------------

static tty_t* _pf_open(const char* tty);
static void   _pf_close(tty_t* tty);
static int    _pf_config(tty_t* tty, tty_cfg_t* cfg);
static int    _pf_read(tty_t* tty, char* buf, int len);
static int    _pf_write(tty_t* tty, char* buf, int len);

//-----------------------------------------------------------------------------
//--------------------------- Public Function ---------------------------------
//-----------------------------------------------------------------------------

tty_t* tty_open(const char* tty){
  return _pf_open(tty);
}

void tty_close(tty_t* tty){
  _pf_close(tty);
}

int tty_config(tty_t* tty, tty_cfg_t* cfg){
  return _pf_config(tty, cfg);
}

int tty_read(tty_t* tty, char* buf, int len){
  return _pf_read(tty, buf, len);
}

int tty_write(tty_t* tty, char* buf, int len){
  return _pf_write(tty, buf, len);
}

//-----------------------------------------------------------------------------
//------------------------- Private function ----------------------------------
//-----------------------------------------------------------------------------

#if defined _WIN32

static tty_t* _pf_open(const char* tty){
  static const tty_cfg_t defcfg = {TTY_BD_9600, TTY_WS_8B, TTY_SB_1B, TTY_PRT_NO};

  int err = 0;
  tty_t* ret = malloc(sizeof(tty_t));
  memset(ret, 0, sizeof(tty_t));

  strcpy(ret->path, "\\\\.\\");
  strcpy(ret->path, tty);

  do{
    ret->fd = CreateFile(ret->path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(ret->fd == INVALID_HANDLE_VALUE) {
      perror("Error open tty");
      err = 1;
      break;
 	  }

    ret->cfg.DCBlength = sizeof(ret->cfg);
    if (!GetCommState(ret->fd, &ret->cfg)) {
      perror("Error get state tty");
      err = 1;
      break;
    }

    if(!GetCommTimeouts(ret->fd, &ret->timeout)){
      perror("Error get timeout tty");
      err = 1;
      break;
    }

    err = _pf_config(ret, &defcfg);
  }while(0);

  if(err){
    _pf_close(ret);
    ret = NULL;
  }

  return ret;
}

static void _pf_close(tty_t* tty){
  CloseHandle(tty->fd);
  free(tty);
}

static int _pf_config(tty_t* tty, tty_cfg_t* cfg){
  int ret = 0;
  int tmp = 0;
  tty->cfg.BaudRate = cfg->BaudRate;
  tty->cfg.ByteSize = cfg->WordSize;
  tty->cfg.StopBits = cfg->StopBit;
  tty->cfg.Parity = cfg->Parity;

  if (!SetCommState(tty->fd, &tty->cfg)) {
    perror("Error set config");
    ret = 1;
  }

  tmp = (200000 / cfg->BaudRate) + 5;
  if(tmp < 20) tmp = 20;

  tty->timeout.ReadIntervalTimeout = tmp / 10;
  tty->timeout.ReadTotalTimeoutConstant = 0;
  tty->timeout.ReadTotalTimeoutMultiplier = 0;
  tty->timeout.WriteTotalTimeoutConstant = tmp / 2;
  tty->timeout.WriteTotalTimeoutMultiplier = 0;

  if (!SetCommTimeouts(tty->fd, &tty->timeout)) {
    perror("Error set timeout");
    ret = 1;
  }

  return ret;
}

static int _pf_read(tty_t* tty, char* buf, int len){
  int ret = 0;
  BOOL isReadSeccess = ReadFile(tty->fd, buf, len, &ret, NULL);
  if(!isReadSeccess) ret = -1;
  return ret;
}

static int _pf_write(tty_t* tty, char* buf, int len){
  int ret = 0;
  BOOL isWriteSuccess = WriteFile(tty->fd, buf, len, &ret, NULL);
  if(!isWriteSuccess) ret = -1;
  return ret;
}

#elif defined __linux__

static tty_t* _pf_open(const char* tty){

}
static void   _pf_close(tty_t* tty){

}
static int    _pf_config(tty_t* tty, tty_cfg_t* cfg){

}
static int    _pf_read(tty_t* tty, char* buf, int len){

}
static int    _pf_write(tty_t* tty, char* buf, int len){
  
}

#else
  #error "Unsupported OS"
#endif

