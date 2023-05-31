#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include <sys/timeb.h>
#include <conio.h>
#include <dos.h>
#include "ssg.c"
#include "pki.c"
#include "numbers.c"
#include "splash.c"

typedef enum{
  BSIZE  =  3,
  BSPEED =  3,
  PSPEED =  5,
  PSIZEX =  5,
  PSIZEY =  25,
  PPAD   =  45,
  MVAL   =  24,
}vars;

#ifndef X
#define X 320
#define Y 200
#endif

void check_quit(){
  if (get_character()==ESC){
    set_gfxMode(VGA_TXT);
    system("cls");
    exit(0);
  }
}

int check_collision(point p, int angle){
  if (p.y <= 2){
    return  360-angle;
  }
  else if (p.y >= 200){
    return  360-angle;
  }
  else{
    return angle;
  }
}
int check_goal(point p){
  if (p.x<= 0){
    return 1;
  }
  else if(p.x>= 320){
    return -1;
  }
  else{
    return 0;
  }
}

int player1_movement(){

  if (check_modifier(ALT)){
    return  PSPEED;
  }
  else if(check_modifier(CTRL)){
    return -PSPEED;
  }
  else{
    return  0;
  }
}
int player2_movement(int *prev){
  int key = get_character();

  switch(key){
    case KD_UP:
      *prev = -PSPEED;
      return -PSPEED;
    case KD_DW:
      *prev = PSPEED;
      return  PSPEED;
    case KR_UP:
      *prev =      0;
      return       0;
    case KR_DW:
      *prev =      0;
      return       0;
  }
  return *prev;
}

int check_Bounds(point p){
  if (p.y <= 0){
    return            0;
  }
  else if (p.y + PSIZEY >=200){
    return 200 - PSIZEY;
  }
  else {
    return p.y;
  }
}
//returns an angle...
int check_Hit(point bp, int angle, point pp1, point pp2){ //BallPos, PaddlePos
  int dy = 0, ny = 0;
  float paddleCenterY; // = paddle.y + paddle.height / 2.0;
  float relativeIntersectY; // = paddleCenterY - ball.y;
  float normalizedRelativeIntersectionY; // = relativeIntersectY / (paddle.height / 2.0);
  float bounceAngle;

  //paddle 1 collision...
  if(bp.x <= pp1.x + PSIZEX && bp.x >= pp1.x){
    if(bp.y+BSIZE >= pp1.y && bp.y <= pp1.y+PSIZEY){
      //please forgive me if I used ChatGPT... I had no other Alternatives...
      paddleCenterY = pp1.y + PSIZEY / 2.0;
      relativeIntersectY = paddleCenterY - bp.y;
      normalizedRelativeIntersectionY = relativeIntersectY / (PSIZEY / 2.0);
      bounceAngle = normalizedRelativeIntersectionY * 60;
      return 180 - angle - bounceAngle;
    }
  }

  //paddle 2 collision...
  else if(bp.x >= pp2.x && bp.x + BSIZE<= pp2.x + PSIZEX){
    if(bp.y <= pp2.y+PSIZEY&&bp.y + BSIZE>= pp2.y){
      paddleCenterY = pp2.y + PSIZEY / 2.0;
      relativeIntersectY = paddleCenterY - bp.y;
      normalizedRelativeIntersectionY = relativeIntersectY / (PSIZEY / 2.0);
      bounceAngle = normalizedRelativeIntersectionY * 60;
      return 180 - angle + bounceAngle;
    }
  }

  return angle;

}

int gen_Angle(){

  int dir = rand()%6;
  switch(dir){
    case 0:
      return   0;
    case 1:
      return  60;
    case 2:
      return 135;
    case 4:
      return 180;
    case 5:
      return 225;
    case 6:
      return 300;
  }
  return 0;
}
float get_MS(){
  return clock();
}

int round(float x){
  return floor(x+0.5);
}

void move_Ball(point *b, int angle){
  double val = 3.141592/180;
  double px = cos(angle*val);
  double py = sin(angle*val);
  double mgtd = sqrt(px*px+py*py);

  px /= mgtd;
  py /= mgtd;

  b->x += round(BSPEED * px);
  b->y += round(BSPEED * py);
}

void check_Pause(int *run){
  char c;
  while(!*run){
    c = getch();
    switch(c){
    case ' ':
      *run = 1;
      break;
    case 'q':
      set_gfxMode(VGA_TXT);
      exit(0);
    }
  }
}

int draw_Score(point sp1, point sp2, int *p1, int *p2, uint8_t *frame){
  uint8_t *nums [10] = {&zero, &one,&two,&three,&four,&five,&six,&seven,&eight,&nine};
  if (*p1 == 10 || *p2 == 10){
    *p1 = 0;
    *p2 = 0;
    draw_Sprite(sp1,18,24,nums[0],frame);
    draw_Sprite(sp2,18,24,nums[0],frame);

    return 0;
  }
  draw_Sprite(sp1,18,24,nums[*p2],frame);
  draw_Sprite(sp2,18,24,nums[*p1],frame);
  return 1;
}

void main(){
  uint8_t *frame   = calloc(X_SIZE*Y_SIZE,sizeof(uint8_t));
  uint8_t *square  = calloc(BSIZE*BSIZE, sizeof(uint8_t));
  uint8_t *paddle  = calloc(PSIZEX*PSIZEY,sizeof(uint8_t));

  point pad1pos;
  point pad2pos;
  point ballpos;
  point score1;
  point score2;

  int run = 1;
  int goal = 0;
  int p1 = 0, p2 = 0, p = 0;
  int *prev = &p;
  float t1 = 0; float t2 = 0; float dt = 0;
  int angle = 0;

  pad1pos.x = PPAD;
  pad1pos.y = 200/2-PSIZEY/2;

  pad2pos.x = 320-PPAD;
  pad2pos.y = 200/2-PSIZEY/2;

  ballpos.x = 160;
  ballpos.y = 100;

  score1.x = 85;
  score1.y = 20;
  score2.x = 320 - 85;
  score2.y = 20;

  srand(time(0));
  angle = gen_Angle();
  memset(paddle,LGRAY,PSIZEX*PSIZEY);
  memset(square,WHITE,BSIZE*BSIZE);
  set_gfxMode(VGA_256_CLR);
  write_Buffer(splash);
  getch();
  for (;;){
    //draw ball and paddles
    set_bgrClr (BLACK,frame);
    run = draw_Score(score1,score2,&p1,&p2,frame);
    draw_Sprite(ballpos,BSIZE,BSIZE,square,frame);
    draw_Sprite(pad1pos,PSIZEX,PSIZEY,paddle,frame);
    draw_Sprite(pad2pos,PSIZEX,PSIZEY,paddle,frame);
    write_Buffer(frame);

    //read joysticks
    pad1pos.y += player1_movement();
    pad2pos.y += player2_movement(prev);

    pad1pos.y = check_Bounds(pad1pos);
    pad2pos.y = check_Bounds(pad2pos);

    move_Ball(&ballpos, angle);


    angle = check_Hit(ballpos, angle, pad1pos, pad2pos);
    //check for any collision
    angle = check_collision(ballpos,angle);

    //check for goal...
    goal = check_goal(ballpos);

    if (goal){
      ballpos.x = 160;
      ballpos.y = 100;

      pad1pos.x = PPAD;
      pad1pos.y = 200/2-PSIZEY/2;

      pad2pos.x = 320-PPAD;
      pad2pos.y = 200/2-PSIZEY/2;

      run = 0;
      angle = gen_Angle();

      switch(goal){
	case 1: //p1 scores
	  p1++;
	  break;

	case -1: //p2 scores
	  p2++;
	  break;
      }
    }

    check_quit();

    check_Pause(&run);
  }

  getch();
  set_gfxMode(VGA_TXT);
}
