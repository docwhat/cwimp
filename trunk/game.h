/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 1999-2000 Christian Höltje

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
#define storVersion 7
#define NumCubes    5  // Number of cubes
#define MaxPlayers 10  // Maximum Number of Players
/* Make sure that MaxName matches the MAXCHARS from the .rcp file */
#define MaxName    32  // Maximum player name size


struct Storage {

  /* These can stay the same if version doesn't change */
  Short   version;
  Short   openingroll; /* This value must be beat to get in the game.      */
  Short   winscore;    /* The score that you must beat to start last licks */
  Short   numplayers;  /* The number of players [1-10]                     */
  Short   numcomputers;/* The number of computers [0-9]                    */
  Short   tmpplayers;  /* Temporary number of players                      */
  Short   tmpcomputers;/* Temporary numebr of 'puters                      */
  Int     flags;       /* For flags, see below.                            */

  /* These are status and counters for during the game and                 *
   * should be reset each time                                             */
  Short   flash;       /* Either 0 or the number of the current flash.     */
  Boolean YMNWTBYM;    /* You May Not Want To, But You Must                */
  Short   leader;      /* -1 if no-one has passed the winscore, or the     *
			* number of the player                             */
  Int     status;      /* Used for picking messages out of statusmsg.c     */
  Short   nTrainWrecks;/* Count for nTrainWrecks rule                      */
  Short   currscore;
  Short   scorethisturn;
  Short   scorethisroll;
  Short   currplayer;
  /* Dice array */
  struct {
    Short  value;
    Short  keep;
  } cube[NumCubes];
  /* Player array */
  struct {
    Boolean computer;
    Boolean lost;
    Char   name[MaxName+1];
    Short  score;
    Short  insurance;
    Short  TWcount;
  } player[MaxPlayers];
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

void LoadCubes(void);
void SaveCubes(void);
void CountCube(void);
void Defaults(void);
void ResetCubes(void);

void Roll(void);
void Stay(void);
void AddScore(Short points);
void ScoreRoll(void);
void TurnLogic(void);

void SetStatus( UInt status );

void NextPlayer(void);
void ToggleKeep(Byte die);
void NewGame(void);
void PlayerWon(void);
void NobodyWon(void);
void PlayerLost( Short player, CharPtr ptrString );

void SetFlag(Int f, Boolean b);

#endif
