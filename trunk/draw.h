#ifndef _DRAW_H_
#define _DRAW_H_
/* $Id$
 * 
 * $Log$
 * Revision 1.1  1999/08/06 05:43:52  palmcvs
 * Initial Revision
 *
 * Revision 0.9  1999/04/11 09:48:25  docwhat
 * Added DialogPreferences
 * Added StatusLine and status help functions
 * flags are now honored in DialogVariants and DialogPreferences
 * preference NextPlayerPopUp works, including popups.
 * Added DialogGetNames to the DialogNewGames.  Names are no
 * longer computer assigned.
 * Cleaned up menus. Added shortcuts for variants and preferences, changed resets.
 *
 * Revision 0.8  1999/03/15 06:19:27  docwhat
 * Added DialogVarients
 * Added MenuItem_Preferences place holder
 * Finished DialogNewGame Handler
 *
 * Revision 0.7  1999/03/14 23:51:36  docwhat
 * Added DialogNewGame.
 * Added hiding Stay button.
 * Added 35 point minimum start.
 *
 * Revision 0.6  1999/03/14 00:55:51  docwhat
 * Seperated out the draw functions
 *
 */

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
