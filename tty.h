#ifndef TTY_H_
#define TTY_H_

#ifdef __cplusplus
extern "C" {
#endif

//struct tty_t;
typedef struct tty_t tty_t;

enum tty_baudrate_e{
  TTY_BD_300     = 300,
  TTY_BD_600     = 600,
  TTY_BD_1200    = 1200,
  TTY_BD_2400    = 2400,
  TTY_BD_4800    = 4800,
  TTY_BD_9600    = 9600,
  TTY_BD_19200   = 19200,
  TTY_BD_38400   = 38400,
  TTY_BD_57600   = 57600,
  TTY_BD_115200  = 115200,
};

enum tty_wordsize_e{
  TTY_WS_8B   = 8,
};

enum tty_stopbit_e{
  TTY_SB_1B = 0,
  TTY_SB_1B5,
  TTY_SB_2B,
};

enum tty_parity_e{
  TTY_PRT_NO  = 0,
  TTY_PRT_ODD,
  TTY_PRT_EVEN,
};

typedef struct tty_cfg_t{
  long BaudRate;    //@tty_baudrate_e
  long WordSize;    //@tty_wordsize_e
  long StopBit;     //@tty_stopbit_e
  long Parity;      //@tty_parity_e
}tty_cfg_t;

tty_t*  tty_open(const char* tty);
void    tty_close(tty_t* tty);
int     tty_config(tty_t* tty, tty_cfg_t* cfg);

int     tty_read(tty_t* tty, char* buf, int len);
int     tty_write(tty_t* tty, char* buf, int len);

#ifdef __cplusplus
}
#endif

#endif //TTY_H_