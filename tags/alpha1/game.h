#ifndef _GAME_H_
#define _GAME_H_
/* $Id$
 * 
 * $Log$
 * Revision 1.1  1999/08/06 05:43:52  palmcvs
 * Initial Revision
 *
 * Revision 0.8  1999/04/11 09:48:25  docwhat
 * Added DialogPreferences
 * Added StatusLine and status help functions
 * flags are now honored in DialogVariants and DialogPreferences
 * preference NextPlayerPopUp works, including popups.
 * Added DialogGetNames to the DialogNewGames.  Names are no
 * longer computer assigned.
 * Cleaned up menus. Added shortcuts for variants and preferences, changed resets.
 *
 * Revision 0.7  1999/03/14 23:51:36  docwhat
 * Added DialogNewGame.
 * Added hiding Stay button.
 * Added 35 point minimum start.
 *
 * Revision 0.6  1999/03/14 00:55:51  docwhat
 * Seperated out the draw functions
 *
 * Revision 0.5  1999/03/13 23:53:28  docwhat
 * Added ident strings.
 *
 */
#include "cwrollerRsc.h"

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

void SetFlag(Int f, Boolean b);

#endif
