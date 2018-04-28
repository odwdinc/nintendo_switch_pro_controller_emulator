#ifndef _PROGRAM_H_
#define _PROGRAM_H_


#include "nintendo.h"




typedef enum {
  X ='a',
  Y,//b
  A,//c
  B,//d
  L,//e
  R,//f
  MINUS,//g
  PLUS,//h
  UP,//i
  DOWN,//j
  LEFT,//k
  RIGHT,//l
  UP_RIGHT,//m
  UP_LEFT,//n
  DOWN_RIGHT,//o
  DOWN_LEFT,//p
  THROW,//q
  TRIGGERS,//r
  NOTHING,//s

} Buttons_t;

typedef struct {
  Buttons_t button;
  uint16_t duration;
} command; 
static const command SetupStep[] = {
    // Setup controller
  { NOTHING,  100 },
  { TRIGGERS,   5 },
  { NOTHING,  50 },
  { TRIGGERS,   5 },
  { NOTHING,  50 },
  { A,         10 },
  { NOTHING,  50 },
};
int SetupStepSize = (int)( sizeof(SetupStep) / sizeof(SetupStep[0]));

//Frog Coine Farming
static const command FrogCoinestep[] = {
  // Find Froge
  { NOTHING,  50 },
  { DOWN,      20 }, // Start Down the hall
  { NOTHING,  10 },
  { R,         5 },
  { NOTHING,  20 },
  { UP,       120 },
  { LEFT,     15 },
  { NOTHING,  10 },
  { UP,       190},
  { LEFT,     10 },
  { Y,        20 },
  { NOTHING,  200 },
  
  { MINUS,     5 },//Eding
  { NOTHING,  40 },
  { UP,       5 },
  { NOTHING,  20 },
  { A,         5 },
  { NOTHING,  500 },
  
};
int FrogCoinestepSize = (int)( sizeof(FrogCoinestep) / sizeof(FrogCoinestep[0]));
#endif

