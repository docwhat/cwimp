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

#include <Pilot.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "cwimp.h"
#include "draw.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"
#include "srand.h"


#include "game.h"


struct Storage stor;
Boolean StayBit; // Normally false, unless player wants to stay

static Char val2name[7][8] = { "Err", /* So I don't have to subtract 1 */
			       "10", "two", "three", "four", "5", "six" };

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
  static int n = 1;

  switch( n++ ) {
  case 1: return 1;
  case 2: return 4;
  case 3: return 5;
  case 4: return 5;
  case 5: return 2;
  default: n=1; break;
  }

  return n;
}
#else
int RollCube (void)
{
  static Boolean init = 0;

  if( init == 0 ) {
    sgenrand( SysRandom(0) );
    init++;
  }

  return ((genrand() %6) + 1);;
}
#endif

/* RollEm -- Rolls all 5 and draws them.
 * Args:    None
 * Returns: None
 */
void Roll(void)
{
  Short x = 0;

  if( stor.currplayer < 0 ) {
    // We don't have a game on
    // We should never get here
    return;
  }

  /* Clear out the score */
  stor.scorethisroll = 0;
  DrawCurrScore();

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
  stor.currscore += points;
}

void ScoreRoll() {
  Short aCounting[7], P1, P2;
  Short BlackDieValue;
  Short x;
  Boolean bool;
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
      PlayerLost( stor.currplayer, "SuperNova: You loose" );
      return;
    }
    if ( x == 6 ) {
      // ToDo: Say why player won
      PlayerWon();
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
      Int  die;
      Int ret;

      DrawCurrScore(); /* So the player knows the score */
      ret = DialogChooseTwo( PickFlashString, 
			     val2name[P1],
			     val2name[P2] );
      if( ret == 1 ) die = P1;
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

  // This is run twice (see below...)
  // Interate through the cubes...
  for( x = 0 ; x < NumCubes ; x++ ) {
    // If the cube isn't kept
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

  // Fill in the Black Die's Flaming Sun
  // if it wasn't used above (in P1 && P2)
  if ( FS && !stor.cube[0].keep ) {
    if ( stor.scorethisroll == 0 ) {
      Int ret;
      Int die;

      DrawCurrScore(); /* Player has to know the score */
      ret = DialogChooseTwo( PickScoreString, "10", "5" );

      if( ret == 1 ) die = 1;
      else die = 5;

      aCounting[die]++;
      BlackDieValue = die;
      stor.cube[0].value = die;
    } else {
      Int ret;
      Int die;

      DrawCurrScore(); /* Player has to know the score */
      ret = DialogChooseThree( PickNonScoreString, "10", "5", NoneString );

      if( ret == 1 ) die = 1;
      else if( ret == 2 ) die = 5;
      else die = 2;
      
      if( die != 2 ) {
	aCounting[die]++;
	BlackDieValue = die;
	stor.cube[0].value = die;
      }
    }
  }


  // Interate through the cubes...
  for( x = 0 ; x < NumCubes ; x++ ) {
    // If the cube isn't kept
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

  /* nTrainWreck rule */
  if( stor.flags & flag_nTW  ) {
    bool = true;
    for( x = 0 ; x < NumCubes ; x++ ) {
      if ( stor.cube[x].keep ) {
	bool = false; // Whups, not a trainwreck
      }
    }
    if( bool && stor.scorethisroll == 0 ) {
      stor.player[stor.currplayer].TWcount++;
      DialogOK( frmTrainWreck, stor.currplayer, -1 );
      if( stor.player[stor.currplayer].TWcount >= stor.nTrainWrecks ) {
	PlayerLost( stor.currplayer, "Too many Train Wrecks" );
      }
    }
  }

}


void TurnLogic() {
  Short kept;
  Short x;

  DrawCurrScore();

  /* 
   * Start looking at the next turn...
   */
  if ( ( stor.scorethisroll == 0 &&
	 !stor.flash ) || 
       StayBit ) {
	
    if ( StayBit ) {
      /* Bump Variation */
      if( stor.flags & flag_Bump ) {
	for ( x = 0 ; x < stor.numplayers ; x++ ) {
	  if ( stor.player[x].score == stor.currscore ) {
	    DialogOK( frmBump, stor.currplayer, x );

	    /* Retribution time */
	    stor.player[x].score = stor.player[stor.currplayer].score;
	    if( stor.leader == x ) {
	      // Don't that suck?
	      stor.leader = -1;
	    }
	    DrawPlayerScore( x );

	    /* There can be only one ... */
	    x = stor.numplayers;
	  }
	}
      }

      stor.player[stor.currplayer].score = stor.currscore;
    }
    
    if( stor.leader == stor.currplayer ) {
      /* We've looped and we're back at the leader */
      PlayerWon();
      return;
    }

    /* Last Licks Stuff */
    if( ! stor.player[stor.currplayer].lost &&
	( stor.currscore > stor.winscore ||
	  stor.leader >= 0 ) ) {
      if( stor.leader < 0 ) {
	/* Hasn't been set, this guy is it */
	stor.leader = stor.currplayer;
	DialogOK( frmLeader, stor.currplayer, -1 );
      } else {
	/* We're in LastLicks already */
	/* There is a leader, did we beat 'em */
	if( stor.currscore > stor.player[stor.leader].score ) {
	  Byte lastleader = stor.leader;

	  stor.leader = stor.currplayer;
	  /* Clean up old bits */
	  DrawPlayerScore( lastleader ); 
	  DialogOK( frmLeader, stor.currplayer, -1 );
	} else {
	  /* Well, we didn't beat the leader... */
	  PlayerLost( stor.currplayer, "Didn't beat the leader" );
	}

      } /* If(stor.leader < 0) ... */
    } /* End Last Licks Stuff */
  
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
  Int prevplayer;

  prevplayer = stor.currplayer;

  while(1) {
    stor.currplayer = (stor.currplayer + 1) % stor.numplayers;
    if ( prevplayer == stor.currplayer ) {
      /* We've looped around or there is only one player */
      if ( stor.numplayers > 1 ) {
	/* If there is more than one player */
	if( !stor.player[stor.currplayer].lost ) {
	  PlayerWon();
	  return;
	} else {
	  NobodyWon();
	  return;
	}
      }
      break;
    }
    if ( ! stor.player[stor.currplayer].lost ) {
      // This player hasn't lost, he's next
      break;
    }
  }

  // Clear cubes
  for( x = 0 ; x < NumCubes ; x++ ) {
    stor.cube[x].keep = false;
    stor.cube[x].value = 0;
  }

  if( stor.currplayer == stor.leader ) {
	PlayerWon();
	return;
  }
  
  if ( stor.flags & flag_NextPlayerPopUp ) {
    //    DialogNextPlayer( prevplayer, stor.currplayer );
    DialogOK( frmNextPlayer, prevplayer, stor.currplayer );
    DrawState();
  } else {
    if( !StayBit ) {
      SetStatus( DS_TurnOver );
      SysTaskDelay(1*sysTicksPerSecond);
    }
    SetStatus( DS_NextPlayer );
  }

  // Clear scores
  stor.scorethisroll =
    stor.scorethisturn = 0;
  stor.currscore = stor.player[stor.currplayer].score;
  StayBit = false;
  DrawState();

}

void PlayerWon() {
  DialogOK( frmWinner, stor.currplayer, -1 );
  ResetCubes();
  DrawState();
  return;
}

void NobodyWon() {
  DialogOK( frmNobodyWon, -1, -1 );
  ResetCubes();
  DrawState();
  return;
}


void PlayerLost( Short player, CharPtr ptrString )
{
  stor.player[player].lost = true;
  
  DialogOK( frmLost, player, -1 );
}


void LoadCubes() {
  Word x,size;

  size = sizeof(stor);

  x = PrefGetAppPreferences( CREATOR, 0, &stor, &size, true);
  if( (x == noPreferenceFound) || 
      (stor.version != storVersion) ) {
    /* This totally resets the whole game. */
    Defaults();
    ResetCubes();
  }

}

void SaveCubes() {
  PrefSetAppPreferences( CREATOR, 0, 1, &stor, sizeof(stor), true );
}

/* Defaults() -- Resets the game to default status
 */
void Defaults(void) {
  Word x;
  
  /* These only get set if the storage structures have changed */
  stor.version = storVersion;
  stor.openingroll = 35;
  stor.numplayers = 1;
  stor.numcomputers = 0;
  stor.winscore = 300;
  stor.flags = 0 | flag_NextPlayerPopUp;
  
  // Clear player names and scores.
  StrCopy( stor.player[0].name, "Scooter" );
  StrCopy( stor.player[1].name, "Fozzie" );
  StrCopy( stor.player[2].name, "Kermit" );
  StrCopy( stor.player[3].name, "Gonzo" );
  StrCopy( stor.player[4].name, "Ms. Piggy" );
  StrCopy( stor.player[5].name, "Animal" );
  StrCopy( stor.player[6].name, "Rowlf" );
  StrCopy( stor.player[7].name, "Robin" );
  StrCopy( stor.player[8].name, "Statler" );
  StrCopy( stor.player[9].name, "Waldorf" );
  
  for (x = 0; x < MaxPlayers; x++) {
    stor.player[x].score = 0;
  }
}

/* ResetCubes -- Resets all the cubes and draws them.
 * Args:    None    
 * Returns: None
 */
void ResetCubes(void)
{
  Short x = 0;

  if ( stor.version != storVersion ) {
    /* These only get set if the storage structures have changed */
    Defaults();
  }

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = 0;
  }

  stor.scorethisturn = 0;
  stor.scorethisroll = 0;
  stor.currscore = 0;
  stor.nTrainWrecks = 3;

  stor.currplayer = -1; // No Game Running

  stor.flash = 0;
  StayBit = false;
  stor.YMNWTBYM = false;

  stor.leader = -1;
  /* DO NOT use SetStatus() here.  It calls draw    *
   * functions which don't work upon initialization */
  stor.status = 0;
}			

void StatusLine( )
{

  if( stor.YMNWTBYM && stor.flash ) {
    SetStatus( DS_YMNWTFlash );
    return;
  }

  if ( stor.YMNWTBYM ) {
    SetStatus( DS_YMNWTBYM );
    return;
  } 

  if ( stor.flash ) {
    SetStatus( DS_MustClearFlash );
    return;
  }

  // Make it blank.
  SetStatus( 0 );
}

void SetStatus( UInt status )
{
  stor.status = status;
  DrawStatus();

}

void NewGame()
{
  Short x;

  stor.flash = 0;
  stor.YMNWTBYM = false;
  stor.leader = -1;
  stor.status = 0;
  stor.currscore = 0;
  stor.scorethisturn = 0;
  stor.scorethisroll = 0;
  stor.currplayer = 0;

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = false;
  }

  for (x = 0; x < stor.numplayers ; x++ ) {
    stor.player[x].computer  = false;
    stor.player[x].lost      = false;
    /* We don't need to set name */
    stor.player[x].score     = 0;
    stor.player[x].insurance = 0;
    stor.player[x].TWcount   = 0;
  }

  StayBit = false;

}


void SetFlag( Int f, Boolean b ) {
  if( b ) {
    stor.flags |= f;
  } else {
    stor.flags &= ~f;
  }
}
