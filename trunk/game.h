/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 2000 Christian H�ltje

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
#include "resource.h"

#define FlamingSun  3  // There isn't a 3 on the BlackDie...

/* Make sure you change the prefVersion if you change the
 * pref struct or any defines needed within it.
 */
#define storVersion 4
#define NumCubes    5  // Number of cubes
#define MaxPlayers 10  // Maximum Number of Players
/* Make sure that MaxName matches the MAXCHARS from the .rcp file */
#define MaxName    32  // Maximum player name size


struct Storage {
  Short   version;
  struct {
	Short  value, keep;
  } cube[NumCubes];
  Short   scorethisturn;
  Short   scorethisroll;
  Short   numplayers;
  Short   numcomputers;
  Short   currplayer;
  struct {
	Boolean computer;
	Boolean lost;
	Char   name[MaxName+1];
	Short  score;
	Short  insurance;
  } player[MaxPlayers];
  Short   flash;       // Either 0 or the number of the current flash.
  Boolean YMNWTBYM;    // You May Not Want To, But You Must
  Short   openingroll; // This value must be beat to get in the game.
  Short   winscore;    // The score that you must beat to start last licks
  Int     status;      // Used for picking messages out of statusmsg.c
  Int     nTrainWrecks;// Count for nTrainWrecks rule
  Int     flags;
};

/*** Flags: */
/* Variants: */
#define flag_Bump            (1<< 0)
#define flag_Eclipse         (1<< 1)
#define flag_Sampler         (1<< 2)
#define flag_nTW             (1<< 3)
#define flag_FullHouse       (1<< 4) 
#define flag_Suspend         (1<< 5)
#define flag_Insurance       (1<< 6)
/* Preferences: */
#define flag_NextPlayerPopUp (1<< 7)

extern struct Storage stor;

extern Boolean StayBit; // Normally false, unless player wants to stay

void LoadCubes();
void SaveCubes();
void CountCube(void);
void ResetCubes(void);

void Roll(void);
void Stay(void);
void AddScore(Short points);
void ScoreRoll();
void TurnLogic();

void StatusLine( );

void NextPlayer();
void ToggleKeep(Byte die);
void NewGame();
void PlayerLost( Short player, CharPtr ptrString );

void SetFlag(Int f, Boolean b);

#endif
