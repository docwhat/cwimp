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

#include "cwimp.h"
#include "data.h"
#include "queue.h"

struct Storage pref;
Boolean StayBit; // Normally false, unless player wants to stay.
Boolean FreezeBit; // Freeze animations, etc. till this is done.
UInt16 SoundAmp; // Set according to sound preferences.

/* ResetCubes -- Resets all the cubes and draws them.
 * Args:    None    
 * Returns: None
 */
void ResetCubes(void)
{
        Int16 x = 0;

        FreezeBit = true;
        EQReset();

        for (x = 0; x < NumCubes; x++) {
                pref.cube[x].value = pref.cube[x].keep = 0;
        }

        pref.scorethisturn = 0;
        pref.scorethisroll = 0;
        pref.currscore = 0;

        pref.clearedscreen = true;

        pref.flash = 0;
        pref.YMNWTBYM = false;

        pref.leader = -1;
        /* DO NOT use SetStatus() here.  It calls draw    *
         * functions which don't work upon initialization */
        pref.status = 0;

        StayBit = false;

        EndGame();
}			

void EndGame(void)
{
        pref.currplayer = -1; // No Game Running
}

void NewGame(void)
{
        Int16 x;

        pref.flash = 0;
        pref.YMNWTBYM = false;
        pref.leader = -1;
        pref.status = 0;
        pref.currscore = 0;
        pref.scorethisturn = 0;
        pref.scorethisroll = 0;
        pref.suspendcount = 0;
        pref.clearedscreen = false;

        for (x = 0; x < NumCubes; x++) {
                pref.cube[x].value = 0;
                pref.cube[x].keep = false;
        }

        pref.currplayer = -1;
        for (x = 0; x < MaxPlayers ; x++ )
        {
                if( pref.player[x].type == PlayerNone )
                {
                        continue;
                }

                if( pref.currplayer < 0 )
                {
                        pref.currplayer = x;
                }
                  
                pref.player[x].lost      		= false;
                pref.player[x].score     		= 0;
                pref.player[x].TWcount   		= 0;
                pref.player[x].suspend.flash		= 0;
                pref.player[x].suspend.status		= 0;
                pref.player[x].suspend.currscore	= 0;
                pref.player[x].suspend.scorethisturn	= 0;
                pref.player[x].suspend.scorethisroll	= 0;
        }

        StayBit = false;
        FreezeBit = false;
}


Boolean isCurrLeader(void)
{
        if( pref.currplayer == pref.leader )
        {
                return true;
        }
        return false;
}

Boolean isGameOn(void)
{
        if( pref.currplayer >= 0 && !pref.clearedscreen )
        {
                return true;
        }
        return false;
}

inline Boolean isCleared(void)
{
        return pref.clearedscreen;
}

Boolean isLostPlayer(Int16 player)
{
        if( pref.player[player].lost )
        {
                return true;
        }
        return false;
}

inline PlayerType GetPlayerType(Int16 player)
{
        return pref.player[player].type;
}

inline UInt16 GetCurrPlayer(void)
{
        return pref.currplayer;
}

UInt16 GetNextPlayer(void)
{
        Int16 p = pref.currplayer + 1;
        
        do
        {
                if( p >= MaxPlayers )
                {
                        p = 0;
                }

                if( p == pref.currplayer )
                {
                        return p;
                }

                if( pref.player[p].type &&
                    !pref.player[p].lost )
                {
                        return p;
                }
                p++;
        } while(1);

        /* How we'd get here, I don't know. */
        return(-1);
}

Char *GetName( Int16 player )
{
        switch ( pref.player[player].type )
        {
        case PlayerHuman:
                return pref.player[player].hname;
                break;
        case PlayerAI:
                return pref.player[player].aname;
                break;
        default:
                return NULL;
                break;
        }
        ErrNonFatalDisplayIf( 1,
                              "CWimp is corrupt, please download a new copy" );
        return NULL;
}

inline
Boolean GetFlag( Int32 flag ) {
        Int32 x;
        x = pref.flags & flag;
        return (pref.flags & flag) > 0;
}  

void SetFlag( Int32 flag, Boolean b ) {
        if( b ) {
                pref.flags |= flag;
        } else {
                pref.flags &= ~flag;
        }
}

Boolean IsAI( Int16 player ) {
        return pref.player[player].type == PlayerAI;
}

void LoadPrefs()
{
  UInt16 x, size;

  /* Initialize EventQueue */
  EQInit();

  size = sizeof(pref);

  /* Load up preferences into stor */
  x = PrefGetAppPreferences( CREATOR, 0, &pref, &size, true);
  if( x != storVersion ) {
    /* This totally resets the whole game. */
    Defaults();
    ResetCubes();
  }

  /* Set the volume levels */
  SndGetDefaultVolume(NULL, NULL, &SoundAmp);

}

void SavePrefs()
{
  PrefSetAppPreferences( CREATOR, 0, storVersion, &pref, sizeof(pref), true );
}

/* Defaults() -- Resets the game to default status
 */
void Defaults(void) {
        UInt16 i;
  
        /* These only get set if the storage structures have changed */
        pref.openingroll = DEFAULT_OPENINGROLL;
        pref.totalplayers = 1;
        pref.clearedscreen = true;
        pref.nTrainWrecks = DEFAULT_nTRAINWRECKS;
        pref.nSuspend = DEFAULT_nSUSPEND;
        pref.winscore = DEFAULT_WINSCORE;
        pref.flags = 0 | flag_NextPlayerPopUp;
  
        // Clear player names and scores.
        for( i = 0; i < MaxPlayers; i++ ) {
                //StrCopy( pref.player[i].name, PlayerDefNames[i] );
                pref.player[i].score = 0;
                pref.player[i].type = PlayerNone;
        }

        pref.player[0].type = PlayerHuman;

        i = 0;
        StrCopy( pref.player[i++].hname, "Scooter" );
        StrCopy( pref.player[i++].hname, "Fozzie" );
        StrCopy( pref.player[i++].hname, "Kermit" );
        StrCopy( pref.player[i++].hname, "Gonzo" );

        StrCopy( pref.player[i++].hname, "Ms. Piggy" );
        StrCopy( pref.player[i++].hname, "Animal" );
        StrCopy( pref.player[i++].hname, "Rowlf" );
        StrCopy( pref.player[i++].hname, "Dr. Teeth" );

        i = 0;
        StrCopy( pref.player[i++].aname, "Eliza" );
        StrCopy( pref.player[i++].aname, "HAL" );
        StrCopy( pref.player[i++].aname, "Kryten" );
        StrCopy( pref.player[i++].aname, "Robbie" );

        StrCopy( pref.player[i++].aname, "Holly" );
        StrCopy( pref.player[i++].aname, "V'Ger" );
        StrCopy( pref.player[i++].aname, "Guardian" );
        StrCopy( pref.player[i++].aname, "Queeg" );

}
