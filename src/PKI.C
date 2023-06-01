#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
/*
	(P)ong (K)eyboard (I)nterface

		 Written by:
	       Ettore Caccioli

		   @2023

https://slickspore.com/

Rev beta 2.0 watcom
*/


int get_character();
int check_modifier(int key);

/* Pong Required Keys */

#define KD_UP  72
#define KD_DW  80
#define KD_LF  75
#define KD_RT  77
#define KR_UP  200
#define KR_LF  203
#define KR_DW  208
#define KR_RT  205
#define KD_SP  185
#define KR_SP  57
#define ALT 3
#define CTRL 2
#define ESC 1

int get_character(){
  //reading directly keyboard interface...
  return inp(0x60);
}
int check_modifier(int key){
  union REGS r;
  
  _asm{
    mov ah,0x02
    mov al,0x00
    int 0x16
  }
  //check for bit set...
  if (r.h.ah & (1 << key)){
      return 1;
  }
  return 0;
}
