/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 1999-2001 Christian Höltje

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#ifndef _DRAW_H_
#define _DRAW_H_

#include "game.h"

// Symbol Font3
#define BlankSymbol "\22" 
#define CheckSymbol "\20"
#define CurrSymbol  "<"
#define OutSymbol   "x"
#define LeadSymbol  "L"
// 20 - dot
// 22 - Empty
// 23 - diamond
// 26 - check
//  3 - left double arrow
//  4 - right double arrow
//  7 - down arrow

#define CubesLeft   2  // Start Drawing Cubes from here
#define CubesTop   19  // From the top, remember that the tab is about 15
#define CubeShift   2  // Space between Cubes
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
void DrawPlayerScore(Short player);
void DrawKeepBit(Byte die);
void DrawTopStatusButton();
void DrawRollButton();
void DrawStayButton();

void DialogNewGame();
void DialogVariants();
void DialogOK( Word frm, Short player1, Short player2 );
Int DialogChooseTwo( CharPtr fText, Int c1, Int c2 );
Int DialogChooseThree( CharPtr fText, Int c1, Int c2, Int c3 );
void DialogPreferences();
Boolean DialogGetNames();
void EnableControl(Word objID, Boolean enable);

#endif
