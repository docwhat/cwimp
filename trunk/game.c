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

#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "cwimp.h"
#include "draw.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"
#include "srand.h"
#include "ai.h"

#include "game.h"


/*
 * Default Names
 */

static
Char **PlayerDefNames = (CharPtr[]){
                           "Scooter",
                           "Fozzie",
                           "Kermit",
                           "Gonzo",
                           "Ms. Piggy",
                           "Animal",
                           "Rowlf",
                           "Robin",
                           "Statler",
                           "Waldorf" };

static
Char **ComputerDefNames = (CharPtr[]){
                             "Eliza",
                             "HAL",
                             "Kryten",
                             "Holly",
                             "Queeg",
                             "Robbie",
                             "DeepThought",
                             "Guardian",
                             "Vger" };


struct Storage stor;
Boolean StayBit; // Normally false, unless player wants to stay

/* RollCube -- Returns a random number from 1 to 6 inclusive.
 * Args: None
 * Returns:
 *     int   -- Number from 1 to 6 inclusive
 */
#ifdef DEBUGROLL
#warning The game is unplayable with DEBUG on
int RollCube (void)
{
  static int n = 1;

  switch( n++ ) {
  case 1: return 2;
  case 2: return 2;
  case 3: return 2;
  case 4: return 2;
  default: n=1; return 2; break;
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

/* Suspend is used by the suspend variation to store the turn */
static void suspend( Short p ) {
  stor.player[p].suspend.flash		= stor.flash;
  stor.player[p].suspend.status		= stor.status;
  stor.player[p].suspend.currscore	= stor.currscore;
  stor.player[p].suspend.scorethisturn	= stor.scorethisturn;
  stor.player[p].suspend.scorethisroll	= stor.scorethisroll;
}

/* Unsuspend does the opposite of Suspend for the same reason */
static void unsuspend( Short p ) {
  stor.flash		= stor.player[p].suspend.flash;
  stor.status		= stor.player[p].suspend.status;
  stor.currscore	= stor.player[p].suspend.currscore;
  stor.scorethisturn	= stor.player[p].suspend.scorethisturn;
  stor.scorethisroll	= stor.player[p].suspend.scorethisroll;
  DrawStatus();
  stor.flash =
    stor.status =
    stor.currscore =
    stor.scorethisturn =
    stor.scorethisroll = 0;
}




/* Roll -- Rolls all 5 and draws them.
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
  if( stor.flags & flag_Suspend ) {
    if( stor.flash > 0 ) {
      stor.suspendcount++;
    } else {
      stor.suspendcount = 0;
    }
    if( stor.suspendcount > stor.nSuspend ) {
      suspend( stor.currplayer );
      NextPlayer();
      return;
    }
  }
  TurnLogic();
}

/* User decided to stay */
void Stay() {
  StayBit = true;
  TurnLogic();
}

/* Add <points> to the current players (temporary) score */
void AddScore(Short points) {
  stor.scorethisroll += points;
  stor.scorethisturn += points;
  stor.currscore += points;
}

/* Score all the points for this roll */
void ScoreRoll() {
  Short aCounting[7];
  Short P1 = 0;
  Short P2 = 0;
  Short BlackDieValue = 0;
  Short x;
  Boolean FS;

  // Init vars
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

      DrawCurrScore(); /* So the player knows the score */
      die = DialogChooseTwo( PickFlashString, P1, P2 );
      
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
      Int die;

      DrawCurrScore(); /* Player has to know the score */
      die = DialogChooseTwo( PickScoreString, 1, 5 );

      aCounting[die]++;
      BlackDieValue = die;
      stor.cube[0].value = die;
    } else {
      Int die;

      DrawCurrScore(); /* Player has to know the score */
      die = DialogChooseThree( PickNonScoreString, 1, 5, 0 );
      if( die == 0 ) die = 2;

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
    Boolean bool = true;
    for( x = 0 ; x < NumCubes ; x++ ) {
      if ( stor.cube[x].keep ) {
	bool = false; // Whups, not a trainwreck
	break;
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

  /* Cosmic Sampler Variant */
  /* This can't happen if we have a Flaming Sun or no score */
  if( (stor.flags & flag_Sampler) &&
      (stor.scorethisroll > 0) &&
      (BlackDieValue == 0) ) {
    Short list[5] = { 0, 0, 0, 0, 0 };
    Boolean bool = true;

    for( x = 0 ; x < NumCubes ; x++ ) {
      if( (stor.cube[x].value == 5) ||
	  (stor.cube[x].value == 1) ) {
	list[5-2]++;
      } else {
	list[stor.cube[x].value-2]++;
      }
    }

    for( x = 0 ; x < 5 ; x++ ) {
      if( list[x] != 1 ) { bool = false;  break; }
    }

    if( bool ) {
      AddScore( 50 );
      DialogOK( frmSampler, stor.currplayer, -1 );
    }
  }
}


void TurnLogic() {
  Short kept;
  Short x;

  DrawCurrScore();

  if( stor.leader == stor.currplayer ) {
    /* Who else is in the game? */
    for( x = 0 ; x < stor.total ; x++ ) {
      if( x == stor.currplayer ) continue;
      if( !stor.player[x].lost ) break;
    }
    /* No one else in the game! */
    if( x == stor.total ) {
      PlayerWon();
      return;
    }
  }
      
  /* The turn has ended */
  if ( ( stor.scorethisroll == 0 &&
	 !stor.flash ) || 
       StayBit ) {
	
    if ( StayBit ) {
      /* Bump Variation */
      if( stor.flags & flag_Bump ) {
	for ( x = 0 ; x < stor.total ; x++ ) {
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
	    x = stor.total;
	  }
	}
      } /* end bump code */

      stor.player[stor.currplayer].score = stor.currscore;
    }
    
    /* Last Licks Stuff */
    if( ! stor.player[stor.currplayer].lost &&
	( stor.player[stor.currplayer].score > stor.winscore ||
	  stor.leader >= 0 ) ) {
      if( stor.leader < 0 ) {
	/* Hasn't been set, this guy is it */
	stor.leader = stor.currplayer;
	DialogOK( frmLeader, stor.currplayer, -1 );
      } else {
	/* We're in LastLicks already */
	/* There is a leader, did we beat 'em? */
	if( stor.player[stor.currplayer].score >
	    stor.player[stor.leader].score ) {
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
  CheckAI();
  return;
}

void NextPlayer() {
  Int x;
  Int prevplayer;

  prevplayer = stor.currplayer;

  while(1) {
    stor.currplayer = (stor.currplayer + 1) % stor.total;
    if ( prevplayer == stor.currplayer ) {
      /* We've looped around or there is only one player */
      if ( stor.total > 1 ) {
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
  
  if( stor.player[prevplayer].suspend.flash > 0 ) {
    DialogOK( frmSuspend, prevplayer, stor.currplayer );
  } else {
    if( stor.flags & flag_NextPlayerPopUp ) {
      DialogOK( frmNextPlayer, prevplayer, stor.currplayer );
    } else {
      if( !StayBit ) {
	SetStatus( DS_TurnOver );
	SysTaskDelay( 1 * SysTicksPerSecond() );
      }
      SetStatus( DS_NextPlayer );
    }
  }

  // Clear scores
  stor.scorethisroll =
    stor.scorethisturn = 0;
  stor.currscore = stor.player[stor.currplayer].score;
  StayBit = false;

  if( stor.flags & flag_Suspend ) {
    stor.suspendcount = 0;
    if( stor.player[stor.currplayer].suspend.flash > 0 ) {
      unsuspend( stor.currplayer );
    }
  }

  DrawState();
  /* so we can process AI stuff */
  CheckAI();
}

void GameEvents(void)
{

  if( stor.flags & flag_PendingAI ) {
    /* Turn This Off! */
    SetFlag( flag_PendingAI, false );
    AITurn();
  }
  
}

void PlayerWon()
{
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

  /* Purge old preferences if they exist */
  PrefSetAppPreferences( OLDCREATOR, 0, 0, NULL, 0, true );

  x = PrefGetAppPreferences( CREATOR, 0, &stor, &size, true);
  if( (x == noPreferenceFound) || 
      (stor.version != storVersion) ) {
    /* This totally resets the whole game. */
    Defaults();
    ResetCubes();
  }

  SetFlag( flag_PendingAI, IsAI( stor.currplayer ) );
}

void SaveCubes() {
  PrefSetAppPreferences( CREATOR, 0, 1, &stor, sizeof(stor), true );
}

/* Defaults() -- Resets the game to default status
 */
void Defaults(void) {
  Word i;
  
  /* These only get set if the storage structures have changed */
  stor.version = storVersion;
  stor.openingroll = 35;
  stor.numplayers = 1;
  stor.numcomputers = 0;
  stor.total = 1;
  stor.nTrainWrecks = 3;
  stor.nSuspend = 10;
  stor.winscore = 300;
  stor.flags = 0 | flag_NextPlayerPopUp;
  
  // Clear player names and scores.
  for( i = 0; i < MaxPlayers; i++ ) {
    StrCopy( stor.player[i].name, PlayerDefNames[i] );
    stor.player[i].score = 0;
  }

}

/* ResetCubes -- Resets all the cubes and draws them.
 * Args:    None    
 * Returns: None
 */
void ResetCubes(void)
{
  Short x = 0;

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = 0;
  }

  stor.scorethisturn = 0;
  stor.scorethisroll = 0;
  stor.currscore = 0;

  stor.currplayer = -1; // No Game Running

  stor.flash = 0;
  StayBit = false;
  stor.YMNWTBYM = false;

  stor.leader = -1;
  /* DO NOT use SetStatus() here.  It calls draw    *
   * functions which don't work upon initialization */
  stor.status = 0;
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
  stor.suspendcount = 0;

  for (x = 0; x < NumCubes; x++) {
    stor.cube[x].value = stor.cube[x].keep = false;
  }

  for (x = 0; x < stor.total ; x++ ) {
    stor.player[x].lost      = false;
    /* We don't need to set name */
    stor.player[x].score     = 0;
    stor.player[x].TWcount   = 0;
    stor.player[x].suspend.flash		= 0;
    stor.player[x].suspend.status		= 0;
    stor.player[x].suspend.currscore		= 0;
    stor.player[x].suspend.scorethisturn	= 0;
    stor.player[x].suspend.scorethisroll	= 0;
  }

  for (x = stor.numplayers ; x < stor.total; x++ ) {
    StrCopy( stor.player[x].name,
             ComputerDefNames[x - stor.numplayers] );
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

Boolean IsAI( Int player ) {
  return (player >= stor.numplayers)?true:false;
}
