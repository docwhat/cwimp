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
#include "resource.h"
#include <Pilot.h>

#define FlamingSun  3  // There isn't a 3 on the BlackDie...

/* Make sure you change the prefVersion if you change the
 * pref struct or any defines needed within it.
 */
#define storVersion 12
#define NumCubes     5  // Number of cubes
#define MaxPlayers   9  // Maximum Number of Players
/* Make sure that MaxName matches the MAXCHARS from the .rcp file */
#define MaxName     32  // Maximum player name size

/* 
 * Defaults
 */
#define DEFAULT_WINSCORE	500
#define DEFAULT_OPENINGROLL	35
#define DEFAULT_nTRAINWRECKS	3
#define DEFAULT_nSUSPEND	10

#ifndef VERSION
#define VERSION "TestOnly"
#endif

struct Storage {

  /* These can stay the same if version doesn't change */
  Short   version;
  Short   openingroll; /* This value must be beat to get in the game.      */
  Short   winscore;    /* The score that you must beat to start last licks */
  Short   numplayers;  /* The number of players [1-10]                     */
  Short   numcomputers;/* The number of computers [0-9]                    */
  Short   total;       /* The total number of players in the game          */
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
  Short   nSuspend;    /* Count for Suspend rule                           */
  Short   suspendcount;/* Current number of flash tries for suspend        */
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
    Boolean lost;
    Char    name[MaxName+1];
    Short   score;
    struct {
      Short   flash;
      Int     status;
      Short   currscore;
      Short   scorethisturn;
      Short   scorethisroll;
    } suspend;
    Short   TWcount;
  } player[MaxPlayers];
};

/*** Flags: */
/* Variants: */
#define flag_Bump            (1<<  0)
#define flag_Eclipse         (1<<  1)
#define flag_Sampler         (1<<  2)
#define flag_nTW             (1<<  3)
#define flag_Suspend         (1<<  4)
/* Preferences: */
#define flag_NoSound         (1<<  6)
#define flag_NextPlayerPopUp (1<<  7)
/* Specials */
#define flag_PendingAI       (1<<  8)
#define flag_CanStay         (1<<  9)
#define flag_FreightTrain    (1<< 10)

#define SND_ERASE_CUBE	1
#define SND_DRAW_CUBE	2

extern struct Storage stor;

extern Boolean StayBit;
extern Boolean FreezeBit;
extern UInt SoundAmp;

void LoadCubes(void);
void SaveCubes(void);
void CountCube(void);
void Defaults(void);
void ResetCubes(void);

void Roll(void);
void Stay(void);
void AddScore(Short points);
void ScoreRoll(Int nop); /* nop is junk */
void TurnLogic(Int nop); /* nop is junk */

void SetStatus( Int status );

void NextPlayer(Int nop); /* nop is junk */

void GameEvents(void);
void NewGame(void);
void PlayerWon(void);
void NobodyWon(void);
void PlayerLost( Short player, CharPtr ptrString );

Boolean IsAI(Int player);
void SetFlag(Int f, Boolean b);
inline Int GetFlag( Int flag );

#endif
