/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 2000 Christian Höltje

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

#include <Pilot.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "cwimp.h"
#include "draw.h"
#include "statusmsg.h"


#include "game.h"


struct Storage stor;
Boolean StayBit; // Normally false, unless player wants to stay

static Char val2name[6][8] = { "10", "one", "two", "three", "four", "5" };

/* ToggleKeep -- Toggles the fieldKeepBit
 * Args: 
 *     int   -- The die to change (0-4)
 * Returns:
 */
void ToggleKeep(Byte die)
{

  stor.cube[die].keep = !stor.cube[die].keep;

  DrawKeepBit(die);
}

/* RollCube -- Returns a random number from 1 to 6 inclusive.
 * Args: None
 * Returns:
 *     int   -- Number from 1 to 6 inclusive
 */
// ToDo: Clean up and make sure numbers are fairly random
#ifdef DEBUGROLL
#warning The game is unplayable with DEBUG on
int RollCube (void)
{
  //  static int n = 2;
  //  n++;
  static int n = 1;
  if( n == 1 ) n = 3 ; else n = 1;

  return n;
}
#else
int RollCube (void)
{
  int n = 0;
  //  char tmp[255]; // Debug

  n = ((SysRandom(0) %6) + 1);

#if 0
  // Can we prove this helps at all?
  while (n == 0)
    {
      n = ((SysRandom(0) % 6) + 1);
    }
#endif
  
  return n;
}
#endif

/* RollEm -- Rolls all 5 and draws them.
 * Args:    None
 * Returns: None
 */
void Roll(void)
{
  // ToDo: Logic for whether player can roll
  Short x = 0;

  if( stor.currplayer < 0 ) {
    // We don't have a game on
    return;
  }

  for (x = 0; x < NumCubes; x++) {
    if (!stor.cube[x].keep) {
      stor.cube[x].value = RollCube();
    } else {
      stor.cube[x].value = 0;
    }
    DrawCube(x);
    DrawKeepBit(x);
  }

  ScoreRoll();
  TurnLogic();
}

void Stay() {
  StayBit = true;
  TurnLogic();

}

void AddScore(Short points) {
  stor.scorethisroll += points;
  stor.scorethisturn += points;
}

void ScoreRoll() {
  Short aCounting[7], P1, P2;
  Short BlackDieValue;
  Short x;
  Boolean FS;

  // Init vars
  BlackDieValue = 0;
  P1 = P2 = 0;
  stor.scorethisroll = 0;
  // FS will only be true if die 0 isn't kept
  // and it's value is FlamingSun
  FS = false;

  // Zero the counting array
  for ( x = 0 ; x < 7 ; x++ ) {
    aCounting[x] = 0;
  }

  // Fill the counting array
  for ( x = 0 ; x < NumCubes ; x++ ) {
    if ( !stor.cube[x].keep ) {
      if ( x == 0 &&
	   stor.cube[x].value == FlamingSun ) {
	FS = true;
      } else {
	// We found a die of value
	aCounting[ stor.cube[x].value ]++;
      } // If it's the FlamingSun
    } // If it's not kept
  }

  // Futless
  // We are FUTLESS, must clear flash
  if ( stor.flash ) {
    if ( aCounting[stor.flash] == 0 ) {
      // Clear the flash
      stor.flash = 0;
    } else {
      // No score
      return;
    }
  }

  /**  Freight Train  **/
  // If a value in our counting array is equal to NumCubes,
  // then we have a freight train. Which cube we use doesn't
  // matter, if all are the same.
  if ( aCounting[ stor.cube[1].value ] == NumCubes ) {
    x = stor.cube[1].value;
    if ( x == 1 ) {
      // ToDo:
      // User Looses due to supernova
      PlayerLost( stor.currplayer, "SuperNova: You loose" );
      return;
    }
    if ( x == 6 ) {
      // ToDo:
      // Instant Winner
      FrmCustomAlert( calertDEBUG,
		      "You're an instant weiner!",
		      "ToDo: add correct you win stuff",
		      " ");
      return;
    }
    
    AddScore( x * 100 );
    stor.YMNWTBYM = true;
    for ( x = 0 ; x < NumCubes ; x++ ) 
      stor.cube[x].keep = true;
   }


  // Look for Flashes (round one, no FlamingSuns)
  for ( x = 1 ; x <= 6 ; x++ ) {
    if ( aCounting[x] >= 3 ) {
      stor.flash = x;
    }
  }
	  
  // Fill P1 & P2
  if ( FS && !stor.flash ) {
    // Look for pairs
    for ( x = 1 ; x <= 6 ; x++ ) {
      if ( aCounting[x] == 2 ) {
	if ( !P1 )
	  P1 = x;
	else
	  P2 = x;
      }
    } 

    // Do we have any pairs to go with our FS?
    if ( P2 ) {
      Int ret, die;
      ret = FrmCustomAlert( calertWhichFlash,
			    val2name[P1 - 1],
			    val2name[P2 - 1],
			    " ");
      if( ret == 0 ) die = P1;
      else           die = P2;
      aCounting[die]++;
      BlackDieValue = die;
      stor.cube[0].value = die;
    } else if ( P1 ) {
      // No choice. Flaming Sun *must* finish the Flash.
      aCounting[P1]++;
      BlackDieValue = P1;
      stor.cube[0].value = P1;
    }

    // Look for Flashes (round two, with FlamingSuns)
    for ( x = 1 ; x <= 6 ; x++ ) {
      if ( aCounting[x] == 3 ) {
	stor.flash = x;
      }
    }

  } // If FlamingSun
  


  // Flash Post; Score Calculation
  if ( stor.flash ) {
    int count;
    aCounting[stor.flash] -= 3;

    /* We decrement thru the Cubes */
    count = 0;
    x = NumCubes - 1;
    if ( P1 || P2 ) {
      // We've used a FlamingSun to
      // complete a Flash
      stor.cube[0].keep = true;
      count++;

    }
    for ( ; x >= 0 && count < 3 ; x-- ) {
      if ( !stor.cube[x].keep &&
	   stor.cube[x].value == stor.flash ) {
	// We found a cube matching the type of Flash
	stor.cube[x].keep = true;
	count++;
      }
    }
    if ( stor.flash == 1 )
      AddScore(100);
    else
      AddScore(stor.flash*10);
  }  
  
  // Clean Up Score

  // Fill in the Black Die's Flaming Sun
  // if it wasn't used above (in P1 && P2)
  if ( FS && !stor.cube[0].keep ) {
    if ( stor.scorethisroll == 0 ) {
      Int ret;
      Int die = 1;
      ret = FrmCustomAlert( calertPickScoreFS, " ", " ", " " );

      if( ret == 0 ) die = 5;
      // else defaults to 1 (aka '10')

      aCounting[die]++;
      BlackDieValue = die;
      stor.cube[0].value = die;
    } else {
      Int ret;
      Int die = 2;
      ret = FrmCustomAlert( calertPickFS, " ", " ", " " );

      if( ret == 0 ) die = 5;
      if( ret == 1 ) die = 1;
      // else defaults to 2

      aCounting[die]++;
      BlackDieValue = die;
      stor.cube[0].value = die;
    }
  }


  for( x = 0 ; x < NumCubes ; x++ ) {
    if ( !stor.cube[x].keep ) {
      if ( stor.cube[x].value == 1 ) {
	stor.cube[x].keep = true;
	AddScore(10);
      }
      if ( stor.cube[x].value == 5 ) {
	stor.cube[x].keep = true;
	AddScore(5);
      }
    }
  }

}


void TurnLogic() {
  Short kept;
  Short x;
  // ToDo: Update()


  DrawCurrScore();

  /* 
   * Player lost turn
   */
  if ( ( stor.scorethisroll == 0 &&
	 !stor.flash ) || 
       StayBit ) {
	
    if ( StayBit ) {
      stor.player[stor.currplayer].score += stor.scorethisturn;
    }

    NextPlayer();
    return;
  }


  stor.YMNWTBYM = false;

  DrawCurrScore();
  for( x = 0 ; x < NumCubes ; x++ )
    DrawKeepBit(x);


  kept = 0;
  for ( x = 0 ; x < NumCubes ; x++ ) {
    if ( stor.cube[x].keep ) {
      kept++;
    }
  }

  if ( kept == 5 ) {
    // You May Not Want To But You Must
    stor.YMNWTBYM = true;
    for ( x = 0 ; x < NumCubes ; x++ ) {
      stor.cube[x].keep = false;
    }
  }

  DrawStayButton();
  StatusLine();
}

void NextPlayer() {
  Int x;
#ifdef DEBUG
  Int dd = 0;
#endif

  x = stor.currplayer;

  while(1) {
    stor.currplayer = (stor.currplayer + 1) % stor.numplayers;
    if ( x == stor.currplayer ) { // We've looped around
      if ( stor.numplayers > 1 ) { // In case someone's solo
	// Only one guy hasn't lost
	//HaveWinner();
	FrmCustomAlert( calertDEBUG,
			"We have a weiner, I mean a winner!",
			"No, I'm not telling you who.",
			"ToDo: Do this correctly." );
      }
      break;
    }
    if ( ! stor.player[stor.currplayer].lost ) {
      // This player hasn't lost, he's next
      break;
    }
#ifdef DEBUG
    ErrNonFatalDisplayIf( ++dd > (MaxPlayers + 4),
                          "NextPlayer: Had to rely on dd loop check!" );
#endif
  }
  
  DrawPlayerScore( x );
  DrawPlayerScore( stor.currplayer );

  // Clear scores
  stor.scorethisroll = stor.scorethisturn = 0;
  if ( StayBit == true ) {
    StayBit = false;
    DrawStayButton();
  }

  // Clear cubes
  for( x = 0 ; x < NumCubes ; x++ ) {
    stor.cube[x].keep = false;
    stor.cube[x].value = 0;
  }

  stor.status = DS_NextPlayer; // Bypass StatusLine();

  if ( stor.flags & flag_NextPlayerPopUp ) {
    FrmCustomAlert( calertNEXTPLAYER,
		    stor.player[stor.currplayer].name,
		    " ", " ");
    DrawState();
  } else {
    DrawStatus();
  }

}

void PlayerLost( Short player, CharPtr ptrString )
{
  // ToDo: Rewrite this thing
  stor.player[player].lost = 1;
  
  FrmCustomAlert( calertDEBUG,
                  ptrString,
                  "ToDo: PlayerLost()",
                  "	" );
  
}


void LoadCubes() {
  Word x,size;
  Char msg[MaxName+1];

  size = sizeof(stor);

  x = PrefGetAppPreferences( CREATOR, 0, &stor, &size, true);
  if( (x == noPreferenceFound) || 
      (stor.version != storVersion) ) {
    ResetCubes();

    // Clear player names and scores.
    for (x = 0; x < MaxPlayers; x++) {
      StrPrintF( msg, "Wimpy %d", x+1 );
      StrCopy( stor.player[x].name, msg );
      stor.player[x].score = 0;
    }
  }

}

void SaveCubes() {
  PrefSetAppPreferences( CREATOR, 0, 1, &stor, sizeof(stor), true );
}


/* ResetCubes -- Resets all the cubes and draws them.
 * Args:    None    
 * Returns: None
 */
void ResetCubes(void)
{
  Short x = 0;

  stor.version = storVersion;

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = 0;
  }

  stor.scorethisturn = 0;
  stor.scorethisroll = 0;
  // We don't need to zero numcomputers and numplayers, we
  // can save the info for next time.

  stor.currplayer = -1; // No Game Running

  stor.flash = 0;
  stor.flags = 0;
  StayBit = false;
  stor.YMNWTBYM = false;
  stor.openingroll = 35;
  stor.winscore = 300;
  stor.status = 0;
}			

void StatusLine( ) {

  stor.status = -1;
  
  if ( stor.YMNWTBYM ) {
    stor.status = DS_YMNWTBYM;
  } 
  else if ( stor.flash ) {
    stor.status = DS_MustClearFlash;
  }
  	
  DrawStatus();
}



void NewGame()
{
  Short x;

  stor.scorethisturn = 0;
  stor.scorethisroll = 0;
  stor.YMNWTBYM = false;

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = false;
  }

  for (x = 0; x < stor.numplayers ; x++ ) {
    stor.player[x].computer  = false;
    stor.player[x].lost      = false;
    stor.player[x].score     = 0;
    stor.player[x].insurance = 0;
  }

  stor.currplayer = 0;
  stor.flash = 0;
  StayBit = false;
}


void SetFlag( Int f, Boolean b ) {
  if( b ) {
    stor.flags |= f;
  } else {
    stor.flags &= ~f;
  }
}
