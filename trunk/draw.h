#ifndef _DRAW_H_
#define _DRAW_H_
/* $Id$ */

#include "game.h"

// Symbol Font3
#define BlankSymbol "\22" 
#define CheckSymbol "\20"
#define ArrowSymbol "\3"
#define OutSymbol   "\7"
// 20 - dot
// 23 - diamond
// 26 - check
//  3 - left arrow

#define CubesLeft   2  // Start Drawing Cubes from here
#define CubesTop   19  // From the top, remember that the tab is about 15
#define CubeShift   5  // Space between Cubes
#define CubeSize   20  // This size Cube should do


// These const's are initialized in draw.c
extern const UInt fieldKeepBit[5];
extern const UInt fieldNamePlayer[MaxPlayers];
extern const UInt fieldScorePlayer[MaxPlayers];
extern const UInt fieldMarkPlayer[MaxPlayers];
extern const UInt fieldGetNamesPlayer[MaxPlayers];
extern const UInt fieldGetNamesLabel[MaxPlayers];
extern const UInt bmpWCube[6];
extern const UInt bmpBCube[6];   



void DrawCurrScore();
void DrawState();
void DrawCube(Byte die);
void DrawPlayerScore(Byte player);
void DrawKeepBit(Byte die);
void DrawStayButton();

void DialogNewGame();
void DialogVarients();
void DialogPreferences();
Boolean DialogGetNames();

#endif
