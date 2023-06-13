/*
	(S)lick (S)pore (G)raphics

		  Written by:
	      
        SlickSpore

		  @2023

  https://slickspore.com/ || https://github.com/slickspore/dos-pong

  Rev 3.0 watcom
*/

#define VGA_SEGMENT 0xA0000L
#define VGA_256_CLR 0x13
#define VGA_TXT     0x03
#define X_SIZE      320
#define Y_SIZE      200
#define VTRACE      0x03da
#define VTCODE      0x08
#define PIT_CHANNEL_0 0x40       // PIT Channel 0 I/O port
#define PIT_COMMAND 0x43         // PIT Command I/O port
#define PIT_FREQUENCY 1193182    // PIT frequency in Hz


typedef struct {
    int x;
    int y;
}point;

typedef enum {
  BLACK  =  0,
  BLUE   =  1,
  GREEN  =  2,
  CYAN   =  3,
  RED    =  4,
  MGNTA  =  5,
  BROWN  =  6,
  LGRAY  =  7,
  DGRAY  =  8,
  LBLUE  =  9,
  LGREEN = 10,
  LCYAN  = 11,
  LRED   = 12,
  LMGNTA = 13,
  YELLOW = 14,
  WHITE  = 15
};

typedef unsigned char uint8_t;
void set_gfxMode (uint8_t mode);
void set_bgrClr  (uint8_t clr, uint8_t *frame_buffer);
void write_Buffer(uint8_t *frame_buffer);
void set_Pixel   (int x, int y, uint8_t clr, uint8_t *frame_buffer);
void draw_Sprite (point p, int x_dim, int y_dim, uint8_t *sprite, uint8_t *frame_buffer);
volatile int TRIGGER = 0;

void interrupt do_FRAME_TICK() {
    // Increment the timer tick count
    TRIGGER = 1;
    // Send an EOI (End of Interrupt) signal to the PIC
    outp(0x20, 0x20);
}

void set_FPS_LIMIT(int FPS) {
    uint16_t divisor = PIT_FREQUENCY / FPS;

    // Send command to Channel 0 of PIT
    outp(PIT_COMMAND, 0x36);     // 0x36 = Channel 0, Latch count value, Mode 3 (Square wave generator)

    // Set the low byte and high byte of the divisor
    outp(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF));
    outp(PIT_CHANNEL_0, (uint8_t)(divisor >> 8));
}
void start_FPS_COUNT(){
    _dos_setvect(0x08, do_FRAME_TICK);

    _asm {
        sti
    }
}

void wait_FRAME_DONE(){
    TRIGGER = 0;while(!TRIGGER){_asm{NOP};}
}

void set_gfxMode(uint8_t mode){
  _asm{
    mov ah, 0x00
    mov al, mode
    int 0x10
  }
}
void set_bgrClr (uint8_t clr, uint8_t *frame_buffer){
  memset(frame_buffer,clr,X_SIZE*Y_SIZE);
}

void write_Buffer(uint8_t *frame_buffer){
  uint8_t *VGA   = (uint8_t*)VGA_SEGMENT;
  while((inp(VTRACE)&VTCODE)==VTCODE);
  while((inp(VTRACE)&VTCODE)==0x00);
  memcpy(VGA,frame_buffer,X_SIZE*Y_SIZE);
  TRIGGER = 0;
}

void set_Pixel(int x, int y, uint8_t clr, uint8_t *frame_buffer){
  *(frame_buffer + (y*X_SIZE+x)) = clr;
}

void draw_Sprite (point p, int x_dim, int y_dim, uint8_t *sprite, uint8_t *frame_buffer){
  int i, j, k = 0;

  for (i = 0; i < y_dim; i++){
    for (j = 0; j < x_dim; j++){
      set_Pixel(j+p.x,i+p.y,sprite[k], frame_buffer);
      k++;
    }
  }
}

void draw_Square(point p, point p1, uint8_t clr, uint8_t *frame_buffer){
  uint8_t i, j;
  int endx, endy;
  if (p1.x >= p.x){
    endx = p1.x - p.x;
    endy = p1.y - p.y;
  }else{
    endx = p.x - p1.x;
    endy = p.y - p1.y;
  }
  for (i = 0; i < endy; i++){
    for (j = 0; j < endx; j++){
      set_Pixel(j+p.x, i+p.y, clr, frame_buffer);
    }
  }

}
