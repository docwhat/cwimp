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
/* #include <SysEvtMgr.h> */

#include "autogen.h"
#include "cwimp.h"
#include "lowlevel.h"
#include "game.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"
#include "draw.h"
#include "data.h"
#include "ai.h"

#include "dialog.h"

static Char **val2name = (CharPtr[]) { NoneString,
                                       TenString,
                                       TwoString,
                                       ThreeString,
                                       FourString,
                                       FiveString,
                                       SixString };

struct {
        PlayerType type;
        Char hname[PLAYERMaxName+1];
        Char aname[PLAYERMaxName+1];
} tmppref[MaxPlayers];

/*****
 ***** Local Functions
 *****
 */
static void ToggleCheck( Word objID, Int flag )
{
        CtlSetValue( GetObjectPtr( objID ),
                     !GetFlag(flag) );
        SetFlag( flag, !GetFlag(flag) );
}

static void NewGameToggleType( Short );

static Short NewGamePlayerCount()
{
        Short x;
        Short count = 0;
        
        for( x = 0; x < MaxPlayers; x++ )
        {
                if( tmppref[x].type != PlayerNone )
                {
                        count++;
                }
        }
        
        return count;
}

static Boolean DialogNewGameHandleEvent (EventPtr e)
{
        Boolean handled = false;
        Char tmpString[5];
        FormPtr frm;
    
        switch (e->eType) {
        case frmOpenEvent:
                frm = FrmGetActiveForm();
                FrmDrawForm(frm);
                handled = true;
                break;

        case penUpEvent:
                if( e->screenX > NewGameUser &&
                    e->screenX < (NewGameUser + NewGameUserSize) &&
                    e->screenY > NewGameTop &&
                    e->screenY < (NewGameTop + 12 * MaxPlayers) )
                {
                        Int p;
                        p = (Int) ( ( e->screenY - NewGameTop ) / 12 );
                        NewGameToggleType( p );
                }
                break;

        case ctlSelectEvent:
                switch(e->data.ctlSelect.controlID)
                {
		
                case btn_Variants_frmNewGame:
                        DialogVariants();
                        break;
                        
                case btn_OK_frmNewGame:
                        handled = !NewGamePlayerCount();
                        break;
                        
                case btn_Default_frmNewGame:
                        StrIToA( tmpString, DEFAULT_WINSCORE );
                        SetFieldTextFromStr( fld_winscore, tmpString );

                        StrIToA( tmpString, DEFAULT_OPENINGROLL );
                        SetFieldTextFromStr( fld_openingroll, tmpString );
                        break;
                }
                break;

        default:
                break;
        }


        return handled;
}


static Boolean DialogVariantsHandleEvent (EventPtr e)
{
        Boolean handled = false;
        FormPtr frm;
  
        switch (e->eType) {
        case frmOpenEvent:
                frm = FrmGetActiveForm();
                FrmDrawForm(frm);
                handled = true;
                break;

        case ctlSelectEvent:
                if( pref.currplayer >= 0 ) {
                        /* The player *cannot* continue a
                           game once they alter variants */
                        ResetCubes(); 
                }
        
                switch(e->data.ctlSelect.controlID) {
                case check_Bump:
                        ToggleCheck( check_Bump, flag_Bump );
                        if( GetFlag(flag_Eclipse) &&
                            GetFlag(flag_Bump) ) {
                                ToggleCheck( check_Eclipse, flag_Eclipse );
                        }
                        break;
                case check_Eclipse:
                        ToggleCheck( check_Eclipse, flag_Eclipse );
                        if( GetFlag(flag_Eclipse) &&
                            GetFlag(flag_Bump ) ) {
                                ToggleCheck( check_Bump, flag_Bump );
                        }
                        break;
                case check_Sampler:
                        ToggleCheck( check_Sampler, flag_Sampler );
                        break;
                case check_nTW:
                        ToggleCheck( check_nTW, flag_nTW );
                        break;
                case check_Suspend:
                        ToggleCheck(check_Suspend, flag_Suspend   );
                        break;
        
                }
                break;
      
        default:
                break;
        }
  
        return handled;
}


static void NewGameSetPlayerName( Int field, Short player)
{
        if( tmppref[player].type == PlayerNone )
        {
                ClearFieldText( field );
                return;
        }

        if( tmppref[player].type == PlayerHuman )
        {
                SetFieldTextFromStr( field, tmppref[player].hname );
                return;
        }

        SetFieldTextFromStr( field, tmppref[player].aname );
        return;
}

static void NewGameGetPlayerName( Int field, Short player)
{
        FormPtr frm = FrmGetActiveForm();
        CharPtr buff;

        if( tmppref[player].type == PlayerNone )
        {
                return;
        }

        buff = FldGetTextPtr(
                FrmGetObjectPtr(
                        frm,
                        FrmGetObjectIndex(
                                frm,
                                fldNGname0+player )
                        )
                );
        if( tmppref[player].type == PlayerHuman )
        {
                StrCopy( tmppref[player].hname, buff );
        }
        if( tmppref[player].type == PlayerAI )
        {
                StrCopy( tmppref[player].aname, buff );
        }
}


static void NewGameToggleType( Short p )
{
        NewGameGetPlayerName( fldNGname0 + p, p );
        tmppref[p].type = (tmppref[p].type + 1) % 3;

        DrawUserType( p, tmppref[p].type );
        NewGameSetPlayerName( fldNGname0 + p, p );

        PlaySound( SND_TOGGLE_TYPE );

}


/*****
 ***** Non Local Functions
 *****
 */

void DialogNewGame() {
        FormPtr prevForm, frm;
        Word hitButton;
        Char tmpString[5];
        CharPtr text;
        Word  fldIndex;
        Int x;
  
        // Save previous form
        prevForm = FrmGetActiveForm();
        FrmSetFocus( prevForm, noFocus );

        // Init new form
        frm = FrmInitForm( frmNewGame );

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);

        for( x = 0; x < MaxPlayers; x++ )
        {
                tmppref[x].type = GetPlayerType( x );
                StrCopy( tmppref[x].hname, pref.player[x].hname );
                StrCopy( tmppref[x].aname, pref.player[x].aname );

                DrawUserType( x, tmppref[x].type );

                NewGameSetPlayerName( fldNGname0+x, x );
        }

        // Fill in WinScore with previous value.
        if ( pref.winscore > 9999 ) { // Sanity check...
                pref.winscore = DEFAULT_WINSCORE;
        }
        StrIToA( tmpString, pref.winscore );
        SetFieldTextFromStr( fld_winscore, tmpString );

        // Fill in Opening Roll with previous value.
        if ( pref.openingroll > pref.winscore || pref.openingroll <= 0 ) {
                // Sanity check...
                pref.openingroll = DEFAULT_OPENINGROLL;
        }
        StrIToA( tmpString, pref.openingroll );
        SetFieldTextFromStr( fld_openingroll, tmpString );

        // Set the focus to this field so the user can just start typing.
        fldIndex =  FrmGetObjectIndex( frm, fldNGname0 );
        FrmSetFocus( frm, fldIndex );

        // Set the handler
        FrmSetEventHandler(frm, DialogNewGameHandleEvent);

        hitButton = FrmDoDialog(frm);

        FrmSetFocus( frm, noFocus );

        if ( hitButton == btn_OK_frmNewGame ) {
                Int num;
    
                pref.winscore = DEFAULT_WINSCORE;
                text = FldGetTextPtr( FrmGetObjectPtr (frm, fldIndex) );
                if ( text != NULL ) {
                        num = StrAToI( text );
                        if( num <= 9999 && num >= 50 ) {
                                pref.winscore = num;
                        }
                }
    
                pref.openingroll = DEFAULT_OPENINGROLL;
                fldIndex =  FrmGetObjectIndex(frm, fld_openingroll);
                text = FldGetTextPtr( FrmGetObjectPtr (frm, fldIndex) );
                if ( text != NULL ) {
                        num = StrAToI( text );
                        if( num < pref.winscore && num > 0 ) {
                                pref.openingroll = num;
                        }
                }


                pref.totalplayers = 0;
                for( x = 0; x < MaxPlayers; x++ )
                {
                        pref.player[x].type = tmppref[x].type;

                        NewGameGetPlayerName( fldNGname0 + x, x );

                        StrCopy( pref.player[x].hname,
                                 tmppref[x].hname );
                        StrCopy( pref.player[x].aname,
                                 tmppref[x].aname );

                        if( tmppref[x].type != PlayerNone )
                        {
                                pref.totalplayers++;
                        }
                }
        }

        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }


        if ( hitButton == btn_OK_frmNewGame ) {
                NewGame();
        }

        DrawState();
}



void DialogVariants() {
        FormPtr prevForm, frm;
        Word hitButton;
        Word fldIndex;
        Char tmpString[3];
        CharPtr text;
  
        // Save previous form
        prevForm = FrmGetActiveForm();
        FrmSetFocus( prevForm, noFocus );

        // Init new form
        frm = FrmInitForm( frmVariants );

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);

        // Set Controls
        CtlSetValue( GetObjectPtr(check_Bump     ), GetFlag(flag_Bump)      );
        CtlSetValue( GetObjectPtr(check_Eclipse  ), GetFlag(flag_Eclipse)   );
        CtlSetValue( GetObjectPtr(check_Sampler  ), GetFlag(flag_Sampler)   );
        CtlSetValue( GetObjectPtr(check_nTW      ), GetFlag(flag_nTW)       );
        CtlSetValue( GetObjectPtr(check_Suspend  ), GetFlag(flag_Suspend)   );

        // Fill in WinScore with previous value.
        StrIToA( tmpString, pref.nTrainWrecks );
        SetFieldTextFromStr( fldnTrainWrecks, tmpString );

        StrIToA( tmpString, pref.nSuspend );
        SetFieldTextFromStr( fldnSuspend, tmpString );

        // Set the focus to this field so the user can just start typing.
        fldIndex =  FrmGetObjectIndex(frm, fldnTrainWrecks);
        FrmSetFocus( frm, fldIndex );

        // Set the handler
        FrmSetEventHandler(frm, DialogVariantsHandleEvent);

        hitButton = FrmDoDialog(frm);

        FrmSetFocus( frm, noFocus );

        // Get Controls
        text = FldGetTextPtr( FrmGetObjectPtr( frm, fldIndex ) );
        if( text != NULL ) {
                pref.nTrainWrecks = StrAToI( text );
        } else {
                pref.nTrainWrecks = 3;
        }    

        fldIndex =  FrmGetObjectIndex(frm, fldnSuspend);
        text = FldGetTextPtr( FrmGetObjectPtr( frm, fldIndex ) );
        if( text != NULL ) {
                pref.nSuspend = StrAToI( text );
        } else {
                pref.nSuspend = 10;
        }    
    
        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }

}


void DialogOK ( Word frmname, Short p1, Short p2 ) {
        FormPtr prevForm, frm;
        Word hitButton;
        Char msg[(PLAYERMaxName * 2) + 128];
        Word fieldname = 0;

        switch ( frmname ) {
        case frmNextPlayer: 
                fieldname = fldNextPlayer;
                if( pref.totalplayers > 1 ) {
                        StrPrintF( msg, NextPlayerString,
                                   GetName( p1 ),
                                   GetName( p1 ),
                                   NULL);
                } else {
                        StrPrintF( msg, "%s", NextSoloPlayerString, NULL );
                }
                break;

        case frmSuspend: 
                if( pref.totalplayers == 1 ) {
                        return;
                }
                fieldname = fldSuspend;
                StrPrintF( msg, SuspendString,
                           GetName( p1 ),
                           GetName( p2 ),
                           NULL);
                break;


        case frmLost:
                fieldname = fldLost;
                StrPrintF( msg, LostString, GetName( p1 ), NULL );
                break;

        case frmLeader:
                fieldname = fldLeader;
                StrPrintF( msg, LeaderString, GetName( p1 ), NULL );
                break;

        case frmWinner:
                fieldname = fldWinner;
                StrPrintF( msg, WinnerString, GetName( p1 ), NULL );
                break;

        case frmNobodyWon:
                fieldname = fldNobodyWon;
                StrPrintF( msg, NobodyWonString, NULL );
                break;

        case frmBump:
                fieldname = fldBump;
                StrPrintF( msg, BumpString,
                           GetName( p2 ),
                           GetName( p1 ),
                           pref.player[p2].score,
                           pref.player[p1].score,
                           NULL );
                break;

        case frmSampler:
                fieldname = fldSampler;
                StrPrintF( msg, SamplerString, GetName( p1 ), NULL );
                break;


        case frmTrainWreck:
                fieldname = fldTrainWreck;
                StrPrintF( msg, TrainWreckString,
                           GetName( p1 ),
                           pref.player[p1].TWcount,
                           pref.nTrainWrecks,
                           NULL );
                break;

        default:
                ErrDisplay( "DialogOK: fall through: Programmer sucks." );
                return;
        }

        // Save previous form
        prevForm = FrmGetActiveForm();
        // Init new form
        frm = FrmInitForm( frmname );

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);


        // Fill it...
        ErrFatalDisplayIf( fieldname == 0,
                           "DialogOK: ZeroField: Programmer sucks." );
        SetFieldTextFromStr( fieldname, msg );

        // Set the handler
        // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

        hitButton = FrmDoDialog(frm);

        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }

        // We don't care about which button, there is only one.
}


Int DialogChooseTwo( CharPtr fText, Int c1, Int c2 ) {
        FormPtr prevForm, frm;
        Word hitButton;

        if( IsAI( pref.currplayer ) ) {
                return AIChooseTwo( c1, c2 );
        }
  
        // Save previous form
        prevForm = FrmGetActiveForm();
        // Init new form
        frm = FrmInitForm( frmChooseTwo );

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);

        // Fill it...
        CtlSetLabel( GetObjectPtr(btnChooseTwo1), val2name[c1] );
        CtlSetLabel( GetObjectPtr(btnChooseTwo2), val2name[c2] );
        SetFieldTextFromStr( fldChooseTwo, fText );

        // Set the handler
        // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

        hitButton = FrmDoDialog(frm);

        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }

        if( hitButton == btnChooseTwo1 ) return c1;
        if( hitButton == btnChooseTwo2 ) return c2;
        return c1;
}


Int DialogChooseThree( CharPtr fText, Int c1, Int c2, Int c3 )
{
			
        FormPtr prevForm, frm;
        Word hitButton;

        if( IsAI( pref.currplayer ) ) {
                return AIChooseThree( c1, c2, c3 );
        }
  
        // Save previous form
        prevForm = FrmGetActiveForm();
        // Init new form
        frm = FrmInitForm( frmChooseThree );

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);

        // Fill it...
        CtlSetLabel( GetObjectPtr(btnChooseThree1), val2name[c1] );
        CtlSetLabel( GetObjectPtr(btnChooseThree2), val2name[c2] );
        CtlSetLabel( GetObjectPtr(btnChooseThree3), val2name[c3] );
        SetFieldTextFromStr( fldChooseThree, fText );

        // Set the handler
        // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

        hitButton = FrmDoDialog(frm);

        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }

        if( hitButton == btnChooseThree1 ) return c1;
        if( hitButton == btnChooseThree2 ) return c2;
        if( hitButton == btnChooseThree3 ) return c3;
        return c1;
}


void DialogPreferences() {
        FormPtr prevForm, frm;
        Word hitButton;
  
        // Save previous form
        prevForm = FrmGetActiveForm();
        // Init new form
        frm = FrmInitForm( frmPreferences);

        // Set it
        FrmSetActiveForm(frm);
        FrmDrawForm(frm);

        // Set Controls
        CtlSetValue( GetObjectPtr(check_NextPlayerPopUp ),
                     GetFlag(flag_NextPlayerPopUp) );

        CtlSetValue( GetObjectPtr(check_ForceSoundOff ),
                     GetFlag(flag_NoSound) );

        // Set the handler
        // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

        hitButton = FrmDoDialog(frm);

        // Get Controls
        SetFlag( flag_NextPlayerPopUp,
                 CtlGetValue( GetObjectPtr(check_NextPlayerPopUp) ) );
        SetFlag( flag_NoSound,
                 CtlGetValue( GetObjectPtr(check_ForceSoundOff) ) );

        // Delete the form, we're not using it
        FrmDeleteForm(frm);

        // Restore previous form.
        if (prevForm) {
                FrmSetActiveForm(prevForm);
        }

        // We don't care, as long as the dialog quits.
}



