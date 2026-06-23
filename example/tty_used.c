#include <stdio.h>
#include <stdlib.h>
#include "tty.h"
#include "sock.h"
#include <string.h>

int main(int argc, char* argv[]){
  
  tty_t* tty = tty_open("COM5");

  if(tty == NULL)
    puts("Not open port");
  else
    puts("port open");

  char input[1024] = {0};
  short it_i = 0;
  while(1){
    int exit = 0;
    while(it_i < sizeof(input) && !exit){
      input[it_i] = getchar();
      if(input[it_i] == '\n')
        exit = 1;
      it_i++;
    }

    int temp = tty_write(tty, input, strlen(input));
    printf("Write data: %i\n", temp);
    memset(input, 0, strlen(input));

    temp = tty_read(tty, input, sizeof(input));
    printf("Read %i byte: %s\n", temp, input);
    memset(input, 0, strlen(input));
    it_i = 0;
  }

  if(tty != NULL)
    tty_close(tty);
  return 0;
}