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

#include "data.h"

#define SND_ERASE_CUBE	1
#define SND_DRAW_CUBE	2
#define SND_TOGGLE_TYPE	3


#define BlankSymbol "\22"
#define CheckSymbol "\20"

void DrawIntro();
void DrawCurrScore();
void DrawState();

void DrawCube(DieType die);
void EraseCube(DieType die);
void DrawBlackCube(DieType die);
void DrawWhiteCube(DieType die);
void CrossCube(DieType die);

void DrawPlayers(void);
void DrawPlayer(UInt8 player);
void ClearKeepBits(void);
void DrawKeepBit(DieType die);
void DrawTopStatusButton();
void DrawStatus();

Boolean DrawFlashies();

void DrawRollButton();
void DrawStayButton();

void PlaySound(DieType type);

void ShowControl(UInt16 objID, Boolean enable);
void ShowButtons(DieType show);
void DrawUserType(UInt16 index, PlayerType type);

#ifdef DEBUG
void EQStatus(UInt16 x);
#endif

#endif
