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

#include <PalmOS.h>

#include "autogen.h"
#include "data.h"
#include "cwimp.h"
#include "draw.h"
#include "dialog.h"
#include "statusmsgstrings.h"
#include "srand.h"
#include "ai.h"
#include "queue.h"

#include "game.h"

/* RollCube -- Returns a random number from 1 to 6 inclusive.
 * Args: None
 * Returns:
 *     int   -- Number from 1 to 6 inclusive
 */
#ifdef DEBUGROLL
#warning The game is unplayable with DEBUGROLL on
DieType RollCube (void)
{
        static DieType n = 1;
        static q = 4;

        switch( n++ ) {
        case 1: return q;
        case 2: return q;
        case 3: return q;
        case 4: return q+2;
        default: n=1; if( q > 6 ) q = 2;
                return q; break;
        }

        return n;
}
#else
DieType RollCube (void)
{
        static Boolean init = 0;

        if( init == 0 ) {
                //seedMT( SysRandom(0) );
                seedMT( TimGetTicks() );
                init++;
        }

        return (randomMT() % 6) + 1;
}
#endif

/* Suspend is used by the suspend variation to store the turn */
static void suspend(Int16 p) {
        pref.player[p].suspend.flash		= pref.flash;
        pref.player[p].suspend.status		= pref.status;
        pref.player[p].suspend.currscore	= pref.currscore;
        pref.player[p].suspend.scorethisturn	= pref.scorethisturn;
        pref.player[p].suspend.scorethisroll	= pref.scorethisroll;
}

/* Unsuspend does the opposite of Suspend for the same reason */
static void unsuspend(Int16 p) {
        pref.flash		= pref.player[p].suspend.flash;
        pref.status		= pref.player[p].suspend.status;
        pref.currscore	= pref.player[p].suspend.currscore;
        pref.scorethisturn	= pref.player[p].suspend.scorethisturn;
        pref.scorethisroll	= pref.player[p].suspend.scorethisroll;
        DrawStatus();
        pref.flash =
                pref.status =
                pref.currscore =
                pref.scorethisturn =
                pref.scorethisroll = 0;
}


/* Roll -- Rolls all 5 and draws them.
 * Args:    None
 * Returns: None
 */
void Roll(void)
{
        Int16 x = 0;

        if( !isGameOn() ) {
                // We don't have a game on
                // We should never get here
                return;
        }

        /* Clear out the score */
        pref.scorethisroll = 0;
        pref.YMNWTBYM = false;
        DrawCurrScore();

        ClearKeepBits();

        /* Clear the dice, mark old ones */
        for(x = 0; x < NumCubes; x++ ) {
                if (!pref.cube[x].keep) {
                        EQAdd( EraseCube, x );
                } else {
                        pref.cube[x].value = 0 - abs(pref.cube[x].value);
                        EQAdd( DrawCube, x );
                }
        }

        /* Roll the new dice */
        for (x = 0; x < NumCubes; x++) {

                if (pref.cube[x].keep) continue;

                pref.cube[x].value = RollCube();

                if( x == 0 ) {
                        EQAdd( DrawBlackCube, x );
                        EQAdd( DrawWhiteCube, x );
                } else {
                        EQAdd( DrawWhiteCube, x );
                        EQAdd( DrawBlackCube, x );
                }
                EQAdd( DrawCube, x );
        }

        // EQAdd( DrawAllCubes );

        EQAdd( ScoreRoll, 0 );
}

/* User decided to stay */
void Stay() {
        StayBit = true;
        EQAdd( TurnLogic, 0);
}

/* Add <points> to the current players (temporary) score */
void AddScore(Int16 points) {
        pref.scorethisroll += points;
        pref.scorethisturn += points;
        pref.currscore += points;
}

/* Score all the points for this roll */
void ScoreRoll(DieType x) {
        Int16	aCounting[7];
        Int16	P1 = 0;
        Int16	P2 = 0;
        Int16	BlackDieValue = 0;
	DieType	die;
        Boolean	FS;

        // Init vars
        pref.scorethisroll = 0;
        // FS will only be true if die 0 isn't kept
        // and it's value is FlamingSun
        FS = false;

        // Zero the counting array
        for ( x = 0 ; x < 7 ; x++ ) {
                aCounting[x] = 0;
        }

        // Fill the counting array
        for ( x = 0 ; x < NumCubes ; x++ ) {
                if ( !pref.cube[x].keep ) {
                        if ( x == 0 &&
                             pref.cube[x].value == FlamingSun ) {
                                FS = true;
                        } else {
                                // We found a die of value
                                aCounting[ pref.cube[x].value ]++;
                        } // If it's the FlamingSun
                } // If it's not kept
        }

        // Futtless
        // We are FUTTLESS, must clear flash
        if ( pref.flash ) {
                if ( aCounting[pref.flash] == 0 ) {
                        // Clear the flash
                        pref.flash = 0;
                } else {
                        // No score - Turn on buttons again
                        for( x = 0; x < NumCubes; x++ ) {
                                if( pref.cube[x].value == pref.flash ) {
                                        EQAdd( CrossCube, x );
                                }
                        }
                        EQAdd( ShowButtons, (DieType)true );
                        return;
                }
        }

        /**  Freight Train  **/
        // If a value in our counting array is equal to NumCubes,
        // then we have a freight train. Which cube we use doesn't
        // matter, if all are the same.
        if ( aCounting[ pref.cube[1].value ] == NumCubes ) {
                x = pref.cube[1].value;
                if ( x == 1 ) {
                        PlayerLost( GetCurrPlayer(), SuperNovaString );
                        return;
                }
                if ( x == 6 ) {
                        // ToDo: Say why player won
                        PlayerWon();
                        return;
                }
    
                AddScore( x * 100 );
                pref.YMNWTBYM = true;
                SetFlag( flag_FreightTrain, true );
                for ( x = 0 ; x < NumCubes ; x++ ) 
                        pref.cube[x].keep = true;
        } else {
                // Look for Flashes (round one, no FlamingSuns)
                for ( x = 1 ; x <= 6 ; x++ ) {
                        if ( aCounting[x] >= 3 ) {
                                pref.flash = x;
                        }
                }
	  
                // Fill P1 & P2
                if ( FS && !pref.flash ) {
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
                                
                                /* So the player knows the score */
                                DrawCurrScore(); 
                                die = DialogChooseTwo( PickFlashString,
                                                       P1, P2 );
        
                                aCounting[die]++;
                                BlackDieValue = die;
                                pref.cube[0].value = die;
                        } else if ( P1 ) {
                                /* No choice.
                                 * Flaming Sun *must* finish the Flash. */
                                aCounting[P1]++;
                                BlackDieValue = P1;
                                pref.cube[0].value = P1;
                        }
      
                        // Look for Flashes (round two, with FlamingSuns)
                        for ( x = 1 ; x <= 6 ; x++ ) {
                                if ( aCounting[x] == 3 ) {
                                        pref.flash = x;
                                }
                        }
                } /* If !FreightTrain */
        } /* If FlamingSun */
  


        // Flash Post; Score Calculation
        if ( pref.flash ) {
                UInt16 count;
                aCounting[pref.flash] -= 3;

                /* We decrement thru the Cubes */
                count = 0;
                x = NumCubes - 1;
                if ( P1 || P2 ) {
                        // We've used a FlamingSun to
                        // complete a Flash
                        pref.cube[0].keep = true;
                        count++;

                }
                for ( ; x >= 0 && count < 3 ; x-- ) {
                        if ( !pref.cube[x].keep &&
                             pref.cube[x].value == pref.flash ) {
                                // We found a cube matching the type of Flash
                                pref.cube[x].keep = true;
                                count++;
                        }
                }
                if ( pref.flash == 1 )
                        AddScore(100);
                else
                        AddScore(pref.flash*10);
        }  
  
        // Clean Up Score

        // This is run twice (see below...)
        // Iterate through the cubes...
        for( x = 0 ; x < NumCubes ; x++ ) {
                // If the cube isn't kept
                if ( !pref.cube[x].keep ) {
                        if ( pref.cube[x].value == 1 ) {
                                pref.cube[x].keep = true;
                                AddScore(10);
                        }
                        if ( pref.cube[x].value == 5 ) {
                                pref.cube[x].keep = true;
                                AddScore(5);
                        }
                }
        }

        // Fill in the Black Die's Flaming Sun
        // if it wasn't used above (in P1 && P2)
        if ( FS && !pref.cube[0].keep ) {
                if ( pref.scorethisroll == 0 ) {
                        DrawCurrScore(); /* Player has to know the score */
                        die = DialogChooseTwo( PickScoreString, 1, 5 );

                        aCounting[die]++;
                        BlackDieValue = die;
                        pref.cube[0].value = die;
                } else {
                        DrawCurrScore(); /* Player has to know the score */
                        die = DialogChooseThree( PickNonScoreString, 1, 5, 0 );
                        if( die == 0 ) die = 2;

                        if( die != 2 ) {
                                aCounting[die]++;
                                BlackDieValue = die;
                                pref.cube[0].value = die;
                        }
                }
        }


        // Iterate through the cubes...
        for( x = 0 ; x < NumCubes ; x++ ) {
                // If the cube isn't kept
                if ( !pref.cube[x].keep ) {
                        if ( pref.cube[x].value == 1 ) {
                                pref.cube[x].keep = true;
                                AddScore(10);
                        }
                        if ( pref.cube[x].value == 5 ) {
                                pref.cube[x].keep = true;
                                AddScore(5);
                        }
                }
        }

        /* nTrainWreck rule */
        if( GetFlag( flag_nTW ) ) {
                Boolean bool = true;
                for( x = 0 ; x < NumCubes ; x++ ) {
                        if ( pref.cube[x].keep ) {
                                bool = false; // Whups, not a trainwreck
                                break;
                        }
                }
                if( bool && pref.scorethisroll == 0 ) {
                        pref.player[GetCurrPlayer()].TWcount++;
                        DialogOK( frmTrainWreck, GetCurrPlayer(), -1 );
                        if( pref.player[GetCurrPlayer()].TWcount >=
                            pref.nTrainWrecks ) {
                                PlayerLost( GetCurrPlayer(), TooManyTWString );
                        }
                }
        }

        /* Cosmic Sampler Variant */
        /* This can't happen if we have a Flaming Sun or no score */
        if( GetFlag( flag_Sampler ) &&
            (pref.scorethisroll > 0) &&
            (BlackDieValue == 0) ) {
                Int16 list[5] = { 0, 0, 0, 0, 0 };
                Boolean bool = true;

                for( x = 0 ; x < NumCubes ; x++ ) {
                        if( (pref.cube[x].value == 5) ||
                            (pref.cube[x].value == 1) ) {
                                list[5-2]++;
                        } else {
                                list[pref.cube[x].value-2]++;
                        }
                }

                for( x = 0 ; x < 5 ; x++ ) {
                        if( list[x] != 1 ) { bool = false;  break; }
                }

                if( bool ) {
                        AddScore( 50 );
                        DialogOK( frmSampler, GetCurrPlayer(), -1 );
                }
        }

        /* Suspend Rule */
        if( GetFlag( flag_Suspend ) ) {
                if( pref.flash > 0 ) {
                        pref.suspendcount++;
                } else {
                        pref.suspendcount = 0;
                }
                if( pref.suspendcount > pref.nSuspend ) {
                        suspend( GetCurrPlayer() );
                        EQAdd( NextPlayer, 0 );
                        return;
                }
        }

        for( x = 0; x < NumCubes; x++ ) {
                EQAdd( DrawKeepBit, x );
        }
        EQAdd( TurnLogic, 0);
} /* End: ScoreRoll() */


void TurnLogic(DieType x) {
        Int16 kept;
        UInt8 lastleader;

        DrawCurrScore();

        if( isCurrLeader() ) {
                /* Who else is in the game? */
                for( x = 0 ; x < pref.totalplayers ; x++ ) {
                        if( x == GetCurrPlayer() ) continue;
                        if( !pref.player[x].lost ) break;
                }
                /* No one else in the game! */
                if( x == pref.totalplayers ) {
                        PlayerWon();
                        return;
                }
        }
      
        /* The turn has ended */
        if ( ( pref.scorethisroll == 0 &&
               !pref.flash ) || 
             StayBit ) {
	
                if ( StayBit ) {
                        /* Bump Variation */
                        if( GetFlag(flag_Bump) ) {
                                for ( x = 0 ; x < pref.totalplayers ; x++ ) {
                                        if ( pref.player[x].score ==
                                             pref.currscore ) {
                                                DialogOK( frmBump,
                                                          GetCurrPlayer(), x );

                                                /* Retribution time */
                                                pref.player[x].score = pref.player[GetCurrPlayer()].score;
                                                if( pref.leader == x ) {
                                                        // Don't that suck?
                                                        pref.leader = -1;
                                                }
                                                DrawPlayer( x );
	
                                                /* There can be only one ... */
                                                x = pref.totalplayers;
                                        }
                                }
                        } /* end bump code */

                        pref.player[GetCurrPlayer()].score = pref.currscore;
                }
    
                /* Last Licks Stuff */
                if( ! pref.player[GetCurrPlayer()].lost &&
                    ( pref.player[GetCurrPlayer()].score >= pref.winscore ||
                      pref.leader >= 0 ) ) {
                        if( pref.leader < 0 ) {
                                /* Hasn't been set, this guy is it */
                                pref.leader = GetCurrPlayer();
                                DialogOK( frmLeader, GetCurrPlayer(), -1 );
                        } else {
                                /* We're in LastLicks already */
                                /* There is a leader, did we beat 'em? */
                                if( pref.player[GetCurrPlayer()].score >
                                    pref.player[pref.leader].score ) {
                                        lastleader = pref.leader;
	  
                                        pref.leader = GetCurrPlayer();
                                        /* Clean up old bits */
                                        DrawPlayer( lastleader ); 
                                        DialogOK( frmLeader, GetCurrPlayer(), -1 );
                                } else {
                                        /* Well, we didn't beat the leader...*/
                                        PlayerLost( GetCurrPlayer(),
                                                    LostToLeaderString );
                                }
	
                        } /* If(pref.leader < 0) ... */
                } /* End Last Licks Stuff */
    
                EQAdd( NextPlayer, 0 );
                return;
        }


        pref.YMNWTBYM = false;

        DrawCurrScore();

        kept = 0;
        for ( x = 0 ; x < NumCubes ; x++ ) {
                if ( pref.cube[x].keep ) {
                        kept++;
                }
        }

        if ( kept == 5 ) {
                // You May Not Want To But You Must
                pref.YMNWTBYM = true;
                for ( x = 0 ; x < NumCubes ; x++ ) {
                        pref.cube[x].keep = false;
                }
        }

        DrawStayButton();
        return;
} /* End: TurnLogic() */

void NextPlayer(DieType x) {
        /* I don't use X, but it's needed for the queue,
           so I use it as throwaway variable. */
        /* Int x; */
        UInt16 prevplayer;

        prevplayer = GetCurrPlayer();
        pref.currplayer = GetNextPlayer();

        if( GetCurrPlayer() == prevplayer && pref.totalplayers > 1 )
        {
                if( isLostPlayer(GetCurrPlayer()) )
                {
                        NobodyWon();
                } else {
                        PlayerWon();
                }
        }

        // Clear cubes
        for( x = 0 ; x < NumCubes ; x++ ) {
                pref.cube[x].keep = false;
                pref.cube[x].value = 0;
        }

        if( isCurrLeader() ) {
                PlayerWon();
                return;
        }
  
        if( pref.player[prevplayer].suspend.flash > 0 ) {
                DialogOK( frmSuspend, prevplayer, GetCurrPlayer() );
        } else {
                if( GetFlag(flag_NextPlayerPopUp) ) {
                        DialogOK( frmNextPlayer, prevplayer, GetCurrPlayer() );
                } else {
                        if( !StayBit ) {
                                EQAdd( SetStatus, DS_Wimpout );
                                EQAdd( EQNOP, 0 );
                                EQAdd( EQNOP, 0 );
                                EQAdd( EQNOP, 0 );
                                EQAdd( EQNOP, 0 );
                                EQAdd( EQNOP, 0 );
                                //EQAdd( SetStatus, DS_NextPlayer );
                                //SysTaskDelay( 1 * SysTicksPerSecond() );
                        }
                        EQAdd( SetStatus, DS_NextPlayer );
                }
        }

        // Clear scores
        pref.scorethisroll =
                pref.scorethisturn = 0;
        pref.currscore = pref.player[GetCurrPlayer()].score;
        StayBit = false;

        if( GetFlag(flag_Suspend) ) {
                pref.suspendcount = 0;
                if( pref.player[GetCurrPlayer()].suspend.flash > 0 ) {
                        unsuspend( GetCurrPlayer() );
                }
        }

        DrawState();
        ShowButtons(true);
}

void GameEvents(void)
{
        Int32 x;

        /* We do *nothing* when we're frozen. */
        if( FreezeBit ) return;

        /* If there is something to do, do it and return */
        if( EQRunNext() ) {
#ifdef DEBUG
                EQStatus(0);
#endif
                return;
        }

        x = GetFlag(flag_PendingAI);
//        if( GetFlag(flag_PendingAI) ) {
        if( x )
        {
                /* Turn This Off! */
                SetFlag( flag_PendingAI, false );
#ifdef DEBUG
                EQStatus(2);
#endif
                AITurn();
                return;
        }
  
#ifdef DEBUG
        EQStatus(1);
#endif
        CheckAI();
        return;
}

void PlayerWon()
{
        DialogOK( frmWinner, GetCurrPlayer(), -1 );
        EndGame();
        DrawState();
        return;
}

void NobodyWon() {
        DialogOK( frmNobodyWon, -1, -1 );
        EndGame();
        DrawState();
        return;
}


void PlayerLost( Int16 player, Char *ptrString )
{
        pref.player[player].lost = true;
        DialogOK( frmLost, player, -1 );
}

void SetStatus( DieType status )
{
        pref.status = status;
        DrawStatus();
}

