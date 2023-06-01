/* **************************************

    This is the second version of LibSSG

	Lib (S)lick (S)pore (G)raphics

	docs on github.com/slickspore

    rev 2.0, OpenWatcom 1.9 much faster than borland...

   ************************************** */


#define VGA_SEGMENT 0xA0000L
#define VGA_256_CLR 0x13
#define VGA_TXT     0x03
#define X_SIZE      320
#define Y_SIZE      200
#define VTRACE      0x03da
#define VTCODE      0x08

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
