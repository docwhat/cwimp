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

#ifndef _GAME_H_
#define _GAME_H_

#include "drawcon.h"
#include "data.h"

#define FlamingSun  3  // There isn't a 3 on the BlackDie...

void LoadCubes(void);
void SaveCubes(void);
void CountCube(void);

void Roll(void);
void Stay(void);
void AddScore(Int16 points);
void ScoreRoll(DieType nop); /* nop is junk */
void TurnLogic(DieType nop); /* nop is junk */

void SetStatus(DieType status );

void NextPlayer(DieType nop); /* nop is junk */

void GameEvents(void);
void NewGame(void);
void PlayerWon(void);
void NobodyWon(void);
void PlayerLost(Int16 player, Char *ptrString );

#endif
