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

#ifndef _DATA_H_
#define _DATA_H_

#include "drawcon.h"

/* Make sure you change the storVersion if you change the
 * pref struct or any defines needed within it.
 */
#define storVersion  4
#define NumCubes     5  // Number of cubes
#define MaxPlayers   8  // Maximum Number of Players

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

typedef enum PlayerEnum {
        PlayerNone  = 0,
        PlayerHuman = 1,
        PlayerAI    = 2
} PlayerType;

typedef Int16	DieType;

struct Storage {
        Int16	openingroll; 
        Int16	winscore;    
        Int16	totalplayers;
        Boolean	clearedscreen; /* Should the screen be cleared? */
        Int32	flags;

        /* These are status and counters for during the game and
         * should be reset each time
         */
        Int16	flash;       /* Either 0 or the number of the current flash. */
        Boolean YMNWTBYM;    /* You May Not Want To, But You Must            */
        Int16	leader;      /* -1 if no-one has passed the winscore, or the *
                              * number of the player                         */
        Int16	status;      /* Used for picking messages out of statusmsg.c */
        Int16	nTrainWrecks;/* Count for nTrainWrecks rule                  */
        Int16	nSuspend;    /* Count for Suspend rule                       */
        Int16	suspendcount;/* Current number of flash tries for suspend    */
        Int16	currscore;
        Int16	scorethisturn;
        Int16	scorethisroll;
        Int16	currplayer;
        /* Dice array */
        struct {
                DieType	value;
                Int16	keep;
        } cube[NumCubes];
        /* Player array */
        struct {
                PlayerType type;
                Boolean lost;
                Char    hname[PLAYERMaxName+1];
                Char    aname[PLAYERMaxName+1];
                Int16  score;
                struct {
                        Int16  flash;
                        Int16  status;
                        Int16  currscore;
                        Int16  scorethisturn;
                        Int16  scorethisroll;
                } suspend;
                Int16  TWcount;
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

/* Global Bits of Data */
extern struct Storage pref;
extern Boolean StayBit;
extern Boolean FreezeBit;
extern UInt16 SoundAmp;


void		ResetCubes(void);
void		EndGame(void);
void		NewGame(void);

Boolean		isCurrLeader(void);
Boolean		isGameOn(void);
Boolean		isCleared(void);
Boolean		isLostPlayer(Int16 player);

PlayerType 	GetPlayerType(Int16 player);
UInt16		GetCurrPlayer(void);
UInt16		GetNextPlayer(void);
Char*		GetName(Int16 player);
Boolean 	IsAI(Int16 player);
void		SetFlag(Int32 flag, Boolean b);
Boolean		GetFlag(Int32 flag);

void		LoadPrefs(void);
void		SavePrefs(void);

void		Defaults(void);


#endif
