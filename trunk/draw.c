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

#include <Pilot.h>
#include <SysEvtMgr.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "autogen.h"
#include "cwimp.h"
#include "lowlevel.h"
#include "game.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"
#include "queue.h"
#include "dialog.h"

#include "drawcon.h"
#include "draw.h"


/* To return the ID of a given die */
static inline UInt getDieID( UInt die )
{
        return idDie0 + die;
}

const UInt bmpWCube[7] = {
        bmpWhite,
        bmp10w,  
        bmp2w,     
        bmp3w,   
        bmp4w,   
        bmp5w,  
        bmp6w
};   
const UInt bmpBCube[7] = {
        bmpBlack,
        bmp10b,
        bmp2b,     
        bmpfsb,   
        bmp4b,   
        bmp5b,  
        bmp6b
};   

/*
 * This function is based on the DrawIntro and DrawBlinds
 * functions in Vexed, a Cool GPL Palm Game by
 * "James McCombe" <cybertube@earthling.net>
 * http://spacetube.tsx.org
 */
void
DrawIntro () {
        VoidHand Title_Handle;
        BitmapPtr Title;
        char text[40];
        SWord penx, peny;
        Boolean bstate;

        /* If a game is on, don't do the splash */
        if( stor.currplayer >= 0 ) return;

        // load bitmap resource and get handle
        Title_Handle = DmGet1Resource ('Tbmp', bmpTitle);
        // lock the bitmap resource into memory and get a pointer to it
        Title = MemHandleLock (Title_Handle);

        // draw the bitmap ( 160x160 )
        WinDrawBitmap (Title, 0, 0);

        /* Text Strings */
        StrPrintF (text, IntroVersionString, VERSION);
        WinDrawChars (text, StrLen (text), 5, 6);

        StrPrintF (text, IntroForPalmString, 153);
        WinDrawChars (text, StrLen (text), 52, 50);

        StrPrintF (text, IntroTapHereString, VERSION);
        WinDrawChars (text, StrLen (text), 45, 85);

        // unload the bitmap from memory (unlock)
        MemHandleUnlock (Title_Handle);

        // Loop till screen is tapped
        while (bstate)
                EvtGetPen (&penx, &peny, &bstate);
        while (!bstate)
                EvtGetPen (&penx, &peny, &bstate);

        /* If the user clicked in the drawing area, then
         * do a Draw Blinds and clear the PenQueue.
         */
        if( peny < 160 ) {
                // DrawBlinds
                int i, x;
                float delay;

                EvtFlushPenQueue();

                delay = .01 * SysTicksPerSecond();

                for (i = 0; i < 16; i++) {
                        for (x = i; x < 160; x += 16) {
                                WinEraseLine( 0,x, 159,x );
                        }
                        SysTaskDelay(delay);
                }
        } /* if( peny < 160 ) */

}

void DrawState()
{
        Short x;

#ifdef DEBUG
        EQStatus(0);
        EQStatus(1);
        EQStatus(2);
        EQStatus(3);

        EQStatus(0);
        EQStatus(1);
        EQStatus(2);
#endif

        DrawCurrScore();

        DrawPlayers();

        for ( x = 0 ; x < NumCubes ; x++ ) {
                DrawCube(x);
                DrawKeepBit(x);
        }

        DrawStayButton();
        DrawRollButton();
        DrawStatus();
        DrawTopStatusButton();
}

void ClearKeepBits(void)
{
        Byte x;
        for( x = 0; x < NumCubes ; x++ ) {
                SetFieldTextFromStr( getDieID( x ),
                                     BlankSymbol);
        }
}

void DrawKeepBit(Int die)
{
  
        if ( stor.YMNWTBYM ||
             (stor.cube[die].keep && stor.cube[die].value > 0) ){
                SetFieldTextFromStr( getDieID(die),
                                     CheckSymbol);
        } else {
                SetFieldTextFromStr( getDieID(die),
                                     BlankSymbol);
        }
}

static inline Int getPLAYERy(Int player)
{
        return PLAYERTLy + player * PLAYEREXy;
}

static void CrossPlayer(Int player) {
        Int y;
        y = getPLAYERy(player) + 6;
        WinDrawLine( PLAYERTLx-3,           y,
                     PLAYERTLx+3+PLAYEREXx, y );
}

static void LeadPlayer(Int player) {
        VoidHand bmpHandle;
        BitmapPtr bmpP;
        Int y = getPLAYERy(player);

        bmpHandle = DmGet1Resource('Tbmp', bmpLeaderIcon);
  
        bmpP = MemHandleLock( bmpHandle );
  
        WinDrawBitmap( bmpP, SYMBOLx, y );
  
        MemHandleUnlock( bmpHandle );
}

static void InvertPlayer(Int player) {
        RectangleType r = { {SYMBOLx, getPLAYERy(player)},
                            {155 - SYMBOLx, PLAYEREXy} };

        WinInvertRectangle( &r, 0 );
}

void DrawPlayer(Int player) {
        Int y;
        Char msg[PLAYERMaxName];
        Char *name;
        RectangleType r = { {SYMBOLx, getPLAYERy(player)},
                            {159 - SYMBOLx, PLAYEREXy} };

        WinEraseRectangle( &r, 0 );

        // Check for a game
        if( stor.currplayer <= -1 )
        {
                return;
        }

        name = stor.player[player].name;

        /* Draw the Name */
        y = PLAYERTLy + player * PLAYEREXy;
        WinDrawChars( name, StrLen( name ),
                      PLAYERTLx, y );

        /* Draw the Score */
        StrIToA( msg, stor.player[player].score );
        WinDrawChars( msg, StrLen( msg ),
                      SCOREx, y );

        if ( stor.player[player].lost )
        {
                CrossPlayer(player);
                return;
        }


        if( player == stor.currplayer )
        {
                InvertPlayer(player);
                return;
        }

        if( stor.leader == player )
        {
                LeadPlayer(player);
                return;
        }

        return;
}

void DrawPlayers() {
        Int x;

        for( x = 0 ; x < MaxPlayers ; x++ )
        {
                DrawPlayer( x );
        }
}

void DrawCurrScore()
{
        Char msg[6];

        if ( stor.flash && stor.scorethisroll == 0 ) {
                StrCopy( msg, NotApplicableString );
        } else {
                StrIToA( msg, stor.scorethisroll );
        }
        SetFieldTextFromStr(scoreRoll,  msg);

        StrIToA( msg, stor.scorethisturn );
        SetFieldTextFromStr(scoreTurn,  msg);

        StrIToA( msg, stor.currscore );
        SetFieldTextFromStr(scoreTot,  msg);

}

static void GreyCube(Int die)
{
        Int i;
        Int yoff, xoff;
        yoff = CUBETLy + (CUBESize + CUBESpace) * die;
        xoff = CUBETLx;

        /* Do the X stuff first */
        for( i = 3; i <= 17; i+=2 ) {
                WinDrawLine( i+xoff,    1+yoff,
                             1+xoff,    i+yoff);
                WinDrawLine(18+xoff,    i+yoff-1,
                            i+xoff-1, 18+yoff);
        }

}

void CrossCube(Int die)
{
        Int yoff, xoff;
  
        yoff = CUBETLy + (CUBESize + CUBESpace) * die;
        xoff = CUBETLx;

        if( die == 0 ) {
                WinEraseLine( 3  +xoff, 4  +yoff,
                              15 +xoff, 16 +yoff);
                WinEraseLine( 3  +xoff, 3  +yoff,
                              16 +xoff, 16 +yoff);
                WinEraseLine( 4  +xoff, 3  +yoff,
                              16 +xoff, 15 +yoff);
  
                WinEraseLine( 3  +xoff, 15 +yoff,
                              15 +xoff, 3  +yoff);
                WinEraseLine( 3  +xoff, 16 +yoff,
                              16 +xoff, 3  +yoff);
                WinEraseLine( 4  +xoff, 16 +yoff,
                              16 +xoff, 4  +yoff);
        } else {

                WinDrawLine( 3  +xoff, 4  +yoff,
                             15 +xoff, 16 +yoff);
                WinDrawLine( 3  +xoff, 3  +yoff,
                             16 +xoff, 16 +yoff);
                WinDrawLine( 4  +xoff, 3  +yoff,
                             16 +xoff, 15 +yoff);
  
                WinDrawLine( 3  +xoff, 15 +yoff,
                             15 +xoff, 3  +yoff);
                WinDrawLine( 3  +xoff, 16 +yoff,
                             16 +xoff, 3  +yoff);
                WinDrawLine( 4  +xoff, 16 +yoff,
                             16 +xoff, 4  +yoff);
        }

}

void DrawCube(Int die)
{
        Int y;
        Int tmpID;

        y = CUBETLy + (CUBESize + CUBESpace) * die;
  
        if ( die == 0 ) { 
                tmpID = bmpBCube[ abs(stor.cube[die].value) ];
        } else {
                tmpID = bmpWCube[ abs(stor.cube[die].value) ];
        }

        DrawBitmap( tmpID, CUBETLx, y );

        if( stor.cube[die].value < 0 ) {
                GreyCube(die);
        }

}

void EraseCube(Int die) {
        RectangleType r = { {CUBETLx, 0}, {CUBESize, CUBESize} };;

        r.topLeft.y = CUBETLy + (CUBESize + CUBESpace) * die;

        WinEraseRectangle( &r, 0 );
        PlaySound( SND_ERASE_CUBE );
}

void DrawBlackCube(Int die)
{

        DrawBitmap( bmpBlack,
                    CUBETLx,
                    CUBETLy + (CUBESize + CUBESpace) * die );

        PlaySound( SND_DRAW_CUBE );
}

void DrawWhiteCube(Int die)
{

        DrawBitmap( bmpWhite,
                    CUBETLx,
                    CUBETLy + (CUBESize + CUBESpace) * die );

        PlaySound( SND_DRAW_CUBE );
}

void DrawRollButton() {
        if( stor.currplayer < 0 ) {
                ShowControl( btn_Roll, 1 );
                CtlSetLabel( GetObjectPtr(btn_Roll), StartString );
        } else {
                if( IsAI( stor.currplayer ) ) {
                        ShowControl( btn_Roll, 0 );
                } else {
                        ShowControl( btn_Roll, 1 );
                        CtlSetLabel( GetObjectPtr(btn_Roll), RollString );
                }
        }
}

void DrawTopStatusButton() {

        if( stor.leader < 0 ) {
                ShowControl( btnTopStatus, 0 );
                return;
        }

        if( stor.leader >= 0 ) {
                ShowControl( btnTopStatus, 1 );
                CtlSetLabel( GetObjectPtr(btnTopStatus), LastLicksString );
                return;
        }

}


/*
 * This should probably go in game.c
 */    
void DrawStayButton() {
        int x;
        /* By default: Show stay button, and set status to nothing */
        Boolean stay = true;
        Int status = 0;

        /* No game, no brainer */
        if( stor.currplayer < 0 ) {
                stay = false;
                goto end;
        }

        if( GetFlag( flag_FreightTrain ) ) {
                SetFlag( flag_FreightTrain, false );
                status = DS_FreightTrain;
                stay = false;
                goto end;
        }

        if( stor.flash ) {
                stay = false;
                status = DS_MustClearFlash;
                goto end;
        }

        if( stor.YMNWTBYM ) {
                stay = false;
                status = DS_YMNWTBYM;
                goto end;
        }

        if( stor.scorethisturn == 0 ) {
                stay = false;
                goto end;
        }
    
        if( stor.player[stor.currplayer].score == 0 &&
            stor.scorethisturn < stor.openingroll )
        {
                status = DS_OpeningRoll;
                stay = false;
                goto end;
        }

        /* If we aren't winning in last licks */
        if( stor.leader >= 0 &&
            ( stor.player[stor.leader].score > stor.currscore ) )
        {
                stay = false;
                goto end;
        }

        if( GetFlag( flag_Eclipse ) ) {
                /* Are we eclipsed? */
                for( x = 0; x < stor.total; x++ ) {
                        if( x == stor.currplayer ) continue;
                        if( stor.player[x].score == stor.currscore ) break;
                }
    
                if( x != stor.total ) {
                        stay = false;
                        status = DS_Eclipse;
                        goto end;
                }
        }

        /* Actually do what we need to do */
end:
        ShowControl( btn_Roll, 1 );
        ShowControl( btn_Stay, stay );
        SetFlag( flag_CanStay, stay );
        SetStatus( status );
}


void PlaySound(Int type) {
        SndCommandType          sndCmd;

        if ( GetFlag( flag_NoSound ) ) return;

        sndCmd.cmd = sndCmdFreqDurationAmp;
        //sndCmd.param2 = 70;
        sndCmd.param2 = 100;
        sndCmd.param3 = SoundAmp;

        /* FREQ:
         *     261.62  C (middle)
         *     293.63  D
         *     349.22  E
         *     391.99  G
         *     440     A
         *     493.88  B
         */

        switch( type ) {
        case SND_ERASE_CUBE:
                sndCmd.param2 = 1;
                sndCmd.param1 = 261.62;
                SndDoCmd( 0, &sndCmd, 0/*noWait*/ );
                break;
        case SND_DRAW_CUBE:
                sndCmd.param2 = 1;
                sndCmd.param1 = 493.88;
                SndDoCmd( 0, &sndCmd, 0/*noWait*/ );
                break;
        default:
                sndCmd.param1 = 440;
                SndDoCmd( 0, &sndCmd, 0/*noWait*/ );
                break;
        }
            

}

void ShowControl(Word objID, Boolean enable) {

        if ( !enable || IsAI( stor.currplayer ) ) {
                CtlHideControl( GetObjectPtr(objID) );
        } else { 
                CtlShowControl( GetObjectPtr(objID) );
        }
  
}

void ShowButtons(Int show) {
        ShowControl(btn_Roll, (Boolean)show);
        if( show && GetFlag(flag_CanStay) ) {
                ShowControl(btn_Stay, true);
        } else {
                ShowControl(btn_Stay, false);
        }
}

#ifdef DEBUG
void EQStatus(Int x)
{
        WinInvertLine( 0 + x*3, 159,
                       1 + x*3, 159);
        WinDrawLine(   0 + x*3, 158,
                       1 + x*3, 158);
}

#endif
