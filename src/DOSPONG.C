/*
	DOS PONG

		  Written by:
	      
        Ettore Caccioli  // I know, it is a lot of code for a single script... sorry if I didn't comment too much...

		  @2023

  https://slickspore.com/ || https://github.com/slickspore/dos-pong

  Rev 3.0 watcom
*/



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
  BSIZE       =  3,
  BSPEED      =  5,
  PSPEED      =  7,
  PSIZEX      =  5,
  PSIZEY      =  25,
  PPAD        =  45,
  MVAL        =  24,
  PBEEP       =  75,
  WBEEP       =  195,
  GAME_FPS    =  50,  //not the real framerate, the real is 33FPS 
  LANES       =  10,
  PADDING     =  5,
  LANE_Y      =  6
}vars;

typedef struct
{
  int end;
  int step;
  int running;

} beep_instance;


#define M_PI 3.141592
#define MAX_ANGLE 120

#ifndef X
#define X 320
#define Y 200
#endif

void start_Beep(beep_instance *b, int freq){
  b->end = 7;
  b->step = 0;
  b->running = 1;
  sound(freq);
}

int round(float x){ return floor(x+0.5);}

void check_quit(){
  if (get_character()==ESC){
    set_gfxMode(VGA_TXT);
    nosound();
    exit(0);
  }
}

int check_collision(point p, beep_instance *b, int angle){
  if (p.y <= 2){
    start_Beep(b, WBEEP);
    return  360-angle;
  }
  else if (p.y >= 200){
    start_Beep(b, WBEEP);
    return  360-angle;
  }
  else{
    return angle;
  }
}
int check_goal(point ball){
  if (ball.x<= 0){
    return 1;
  }
  else if(ball.x>= 320){
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
int player2_movement(int *previews_keyCode){
  int key = get_character();

  switch(key){
    case KD_LF:
      *previews_keyCode = -PSPEED;
      return -PSPEED;
    case KD_RT:
      *previews_keyCode = PSPEED;
      return  PSPEED;
    case KR_LF:
      *previews_keyCode =      0;
      return       0;
    case KR_RT:
      *previews_keyCode =      0;
      return       0;
  }
  return *previews_keyCode;
}

int check_Bounds(point paddle){
  if (paddle.y <= 0){ //upmost
    return            0;
  }
  else if (paddle.y + PSIZEY >=200){
    return 200 - PSIZEY; //downmost
  }
  else {
    return paddle.y;
  }
}
//returns an angle...
int check_Hit(point *bp1, int angle, point pp1, point pp2, beep_instance *b){ //BallPos, PaddlePos

  point bp = *bp1;
  int dy = 0, ny = 0;
  //old vars...
  float relativeIntersectY; // = paddleCenterY - ball.y;
  float normalizedRelativeIntersectionY; // = relativeIntersectY / (paddle.height / 2.0);
  float bounceAngle;
  float incidenceAngle;
  float difference = 0;  //check wheather the ball hits the paddle on top of it or on the bottom...
  float middlePoint;  //please forgive me if I used ChatGPT... I had no other Alternatives...
  //new vars...
  float multiplierAngle;
  float pmiddle;
  float bmiddle;
  float variation;
  float delta;

  //paddle 1 collision...
  if(bp.x <= pp1.x + PSIZEX && bp.x >= pp1.x){
    if(bp.y+BSIZE >= pp1.y && bp.y <= pp1.y+PSIZEY){

      if (bp.y + (BSIZE / 2) <= pp1.y + (PSIZEY / 2)){
        difference = BSIZE;
      }else if (bp.y + (BSIZE / 2) >= pp1.y + (PSIZEY / 2)){
        difference = 0;
      }

      start_Beep(b, PBEEP);
      pmiddle = pp1.y + (PSIZEY / 2);
      bmiddle = bp.y  + difference;
      delta = pmiddle - bmiddle;
      variation = delta / PSIZEY;

      if (variation < 0){
        multiplierAngle = -variation * MAX_ANGLE;
      }else{
        multiplierAngle = 360 - (variation * MAX_ANGLE);
      }

      bp1->x = pp1.x + PSIZEX + 1;

      return round(multiplierAngle); 

    }
  }
  //paddle 2 collision...
  else if(bp.x + BSIZE >= pp2.x && bp.x <= pp2.x + PSIZEX){
    if(bp.y <= pp2.y+PSIZEY&&bp.y + BSIZE>= pp2.y){
      
      if (bp.y + (BSIZE / 2) <= pp2.y + (PSIZEY / 2)){
        difference = BSIZE;
      }else if (bp.y + (BSIZE / 2) >= pp2.y + (PSIZEY / 2)){
        difference = 0;
      }

      start_Beep(b, PBEEP);
      pmiddle = pp2.y + (PSIZEY / 2);
      bmiddle = bp.y  + difference;
      delta = pmiddle - bmiddle;
      variation = delta / PSIZEY;

      if (variation < 0){
        multiplierAngle = 190 + variation * MAX_ANGLE ;
      }else{
        multiplierAngle = 170 + (variation * MAX_ANGLE);
      }


      bp1->x = pp2.x - BSIZE;

      return round(multiplierAngle); 

    }
  }

  return angle;

}

int gen_Angle(){
  int dir = rand()%6;
  int angles[7] = {0,60,135,180,225,300};
  return angles[dir];
}

void move_Ball(point *ball, int angle, time_t dt){
  double val = M_PI/180;
  double delta_x = cos(angle*val);
  double delta_y = sin(angle*val);

  ball->x += round(BSPEED * delta_x);
  ball->y += round(BSPEED * delta_y);
}

void check_Pause(int *run){
  int keyCode;
  while(!*run){
    keyCode = get_character();
    switch(keyCode){
      case ESC:
        set_gfxMode(VGA_TXT);
        exit(0);        
      case KD_SP:
        keyCode = get_character();
        while(keyCode!=KR_SP){keyCode = get_character();if (keyCode == ESC){*run = 1; break;}}
        *run = 1;
        break;
    }

  }
  return;
}

int draw_Score(point p1_score_pos, point p2_score_pos, int *score_P1, int *score_P2, uint8_t *frame){
  uint8_t *numbers [10] = {&zero,&one,&two,&three,&four,&five,&six,&seven,&eight,&nine};
  if (*score_P1 == 10 || *score_P2 == 10){
    *score_P1 = 0;
    *score_P2 = 0;
    draw_Sprite(p1_score_pos,18,24,numbers[0],frame);
    draw_Sprite(p2_score_pos,18,24,numbers[0],frame);

    return 0;
  }
  draw_Sprite(p1_score_pos,18,24,numbers[*score_P2],frame);
  draw_Sprite(p2_score_pos,18,24,numbers[*score_P1],frame);
  return 1;
}

void sound_Tick(beep_instance *b){

  if (b->running && b->end == b->step){
    nosound();
    b->step = 0;
    b->running = 0;
    b->end = 0;
  }
  if (b->running){
    b->step++;
  }

}

time_t get_Time(){
  struct timeb t;
  ftime(&t);
  return t.time * 1000 + (time_t)t.millitm;
}

void draw_Background(uint8_t *frame){
  point lInit;
  point lEnd;
  int i;
  int y_dimension = (Y/LANES);
  int x_pad = LANE_Y / 2;

  lInit.x = X/2 - x_pad;
  lEnd.x  = X/2 + x_pad;
  lInit.y = 0;
  lEnd.y = y_dimension - PADDING;

  for (i = 0; i < LANES; i++){
    draw_Square(lInit, lEnd, LGRAY, frame);
    lInit.y += y_dimension;
    lEnd.y  += y_dimension;
  }
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

  beep_instance sound_instance;

  int run = 1;
  int goal = 0;
  int p1 = 0, p2 = 0;
  int prev = 0;
  int angle = 0;
  time_t t1, t2, dt = 0;

  sound_instance.end = 0;
  sound_instance.running = 0;
  sound_instance.step = 0;

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

  set_FPS_LIMIT(GAME_FPS);
  start_FPS_COUNT();
  set_gfxMode(VGA_256_CLR);

  write_Buffer(splash);
  
  getch();
  for (;;){
    write_Buffer(frame);
    wait_FRAME_DONE();

    //draw ball and paddles and background
    set_bgrClr (BLACK,frame);
    draw_Background(frame);
    run = draw_Score(score1,score2,&p1,&p2,frame);
    draw_Sprite(ballpos,BSIZE,BSIZE,square,frame);
    draw_Sprite(pad1pos,PSIZEX,PSIZEY,paddle,frame);
    draw_Sprite(pad2pos,PSIZEX,PSIZEY,paddle,frame);
    
    //read joysticks
    pad1pos.y += player1_movement();
    pad2pos.y += player2_movement(&prev);

    pad1pos.y = check_Bounds(pad1pos);
    pad2pos.y = check_Bounds(pad2pos);

    move_Ball(&ballpos, angle, dt);


    angle = check_Hit(&ballpos, angle, pad1pos, pad2pos, &sound_instance);
    //check for any collision
    angle = check_collision(ballpos, &sound_instance, angle);

    //check for goal...
    goal = check_goal(ballpos);

    if (goal){
      nosound();
      sound(WBEEP);
      nosound();

      prev = 0;
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

    sound_Tick(&sound_instance);

    check_Pause(&run);
    
  }

  getch();
  set_gfxMode(VGA_TXT);
}
