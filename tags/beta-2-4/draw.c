/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 1999-2001 Christian H�ltje

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

#include "callback.h"
#include "cwimp.h"
#include "lowlevel.h"
#include "game.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"
#include "ai.h"
#include "queue.h"

#include "draw.h"

/* Local functions */
static Boolean DialogNewGameHandleEvent (EventPtr e);
static void ShowControl(Word objID, Boolean enable);
static VoidPtr GetObjectPtr (Word objID);


/* Initialize extern structures */
const UInt fieldKeepBit[5] = { kdie0, kdie1, kdie2, kdie3, kdie4 };
const UInt fieldNamePlayer[MaxPlayers] = {
  namePlayer0,
  namePlayer1,
  namePlayer2,
  namePlayer3,
  namePlayer4,
  namePlayer5,
  namePlayer6,
  namePlayer7,
  namePlayer8,
  namePlayer9
};
const UInt fieldScorePlayer[MaxPlayers] = {
  scorePlayer0,
  scorePlayer1,
  scorePlayer2,
  scorePlayer3,
  scorePlayer4,
  scorePlayer5,
  scorePlayer6,
  scorePlayer7,
  scorePlayer8,
  scorePlayer9
};
const UInt fieldMarkPlayer[MaxPlayers] = {
  markPlayer0,
  markPlayer1,
  markPlayer2,
  markPlayer3,
  markPlayer4,
  markPlayer5,
  markPlayer6,
  markPlayer7,
  markPlayer8,
  markPlayer9
};
const UInt fieldGetNamesPlayer[MaxPlayers] = {
  GnameP0,
  GnameP1,
  GnameP2,
  GnameP3,
  GnameP4,
  GnameP5,
  GnameP6,
  GnameP7,
  GnameP8,
  GnameP9
};
const UInt fieldGetNamesLabel[MaxPlayers] = {
  GnameL0,
  GnameL1,
  GnameL2,
  GnameL3,
  GnameL4,
  GnameL5,
  GnameL6,
  GnameL7,
  GnameL8,
  GnameL9
};

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
const UInt pbtnVal[10] = {
  pbtn_1,
  pbtn_2,
  pbtn_3,
  pbtn_4,
  pbtn_5,
  pbtn_6,
  pbtn_7,
  pbtn_8,
  pbtn_9,
  pbtn_10
};
const UInt cbtnVal[10] = {
  cbtn_0,
  cbtn_1,
  cbtn_2,
  cbtn_3,
  cbtn_4,
  cbtn_5,
  cbtn_6,
  cbtn_7,
  cbtn_8,
  cbtn_9
};

static Char **val2name = (CharPtr[]) { NoneString,
                                       TenString,
                                       TwoString,
                                       ThreeString,
                                       FourString,
                                       FiveString,
                                       SixString };

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
  Char msg[MaxName+4];

#ifdef DEBUG
  EQStatus(0);
  EQStatus(1);
  EQStatus(2);
  EQStatus(3);

  EQStatus(0);
  EQStatus(1);
  EQStatus(2);
  EQStatus(3);
#endif

  DrawCurrScore();

  // Fill the fields
  x = 0;
  if ( stor.currplayer > -1 ) {
	// If there is a game on, fill in the names and scores
	for ( ; x < stor.total ; x++ ) {
	  StrPrintF( msg, "%d. %s", x+1, stor.player[x].name, NULL ); 
	  SetFieldTextFromStr( fieldNamePlayer[x], msg );
	  DrawPlayerScore( x );
	}
  }
  // Clears the rest if the fields
  for ( ; x < MaxPlayers ; x++ ) {
	ClearFieldText( fieldNamePlayer[x] );
	ClearFieldText( fieldScorePlayer[x] );
	ClearFieldText( fieldMarkPlayer[x] );
  }

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
    SetFieldTextFromStr( fieldKeepBit[x],
                         BlankSymbol);
  }
}

void DrawKeepBit(Int die)
{
  
  if ( stor.YMNWTBYM ||
       (stor.cube[die].keep && stor.cube[die].value > 0) ){
	SetFieldTextFromStr(fieldKeepBit[die],
						CheckSymbol);
  } else {
	SetFieldTextFromStr(fieldKeepBit[die],
						BlankSymbol);
  }
}

void DrawPlayerScore(Short player) {
  Char msg[MaxName];

  StrIToA( msg, stor.player[player].score );
  SetFieldTextFromStr( fieldScorePlayer[player], msg );

  if ( stor.player[player].lost ) {
    SetFieldTextFromStr( fieldMarkPlayer[player], OutSymbol );
    return;
  }

  if ( player == stor.currplayer ) {
    SetFieldTextFromStr( fieldMarkPlayer[player], CurrSymbol );
    return;
  }

  if ( player == stor.leader ) {
    SetFieldTextFromStr( fieldMarkPlayer[player], LeadSymbol );
    return;
  }

  ClearFieldText( fieldMarkPlayer[player] );
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
  yoff = CubesTop + (CubeSize + CubeShift) * die;
  xoff = CubesLeft;

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
  
  yoff = CubesTop + (CubeSize + CubeShift) * die;
  xoff = CubesLeft;

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

#if 0
void InvertCube(Int die)
{
  Int y;
  Int tmpID;

  y = CubesTop + (CubeSize + CubeShift) * die;
  
  if ( die != 0 ) { 
    tmpID = bmpBCube[ abs(stor.cube[die].value) ];
  } else {
    tmpID = bmpWCube[ abs(stor.cube[die].value) ];
  }

  DrawBitmap( tmpID, CubesLeft, y );

  if( stor.cube[die].value < 0 ) {
    GreyCube(die);
  }

}
#endif

void DrawCube(Int die)
{
  Int y;
  Int tmpID;

  y = CubesTop + (CubeSize + CubeShift) * die;
  
  if ( die == 0 ) { 
    tmpID = bmpBCube[ abs(stor.cube[die].value) ];
  } else {
    tmpID = bmpWCube[ abs(stor.cube[die].value) ];
  }

  DrawBitmap( tmpID, CubesLeft, y );

  if( stor.cube[die].value < 0 ) {
    GreyCube(die);
  }

}

void DrawBlackCube(Int die)
{

  DrawBitmap( bmpBlack,
              CubesLeft,
              CubesTop + (CubeSize + CubeShift) * die );

}

void DrawWhiteCube(Int die)
{

  DrawBitmap( bmpWhite,
              CubesLeft,
              CubesTop + (CubeSize + CubeShift) * die );

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

  if( stor.flags & flag_FreightTrain ) {
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

  if( stor.flags & flag_Eclipse ) {
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

  // Set Controls
  // cbtnVal & pbtnVal
  stor.tmpplayers = stor.numplayers;
  stor.tmpcomputers = stor.numcomputers;
  if ( stor.numplayers > 0 ) {
	x = stor.numplayers;
  } else {
	x = 1;
  }
  CtlSetValue( GetObjectPtr( pbtnVal[x - 1] ), true );

  if ( stor.numcomputers > 0 ) {
	x = stor.numcomputers;
  } else {
	x = 0;
  }
  CtlSetValue( GetObjectPtr( cbtnVal[x] ), true );

  // Fill in WinScore with previous value.
  if ( stor.winscore > 9999 ) { // Sanity check...
	stor.winscore = DEFAULT_WINSCORE;
  }
  StrIToA( tmpString, stor.winscore );
  SetFieldTextFromStr( fld_winscore, tmpString );

  // Fill in Opening Roll with previous value.
  if ( stor.openingroll > stor.winscore || stor.openingroll <= 0 ) {
    // Sanity check...
    stor.openingroll = DEFAULT_OPENINGROLL;
  }
  StrIToA( tmpString, stor.openingroll );
  SetFieldTextFromStr( fld_openingroll, tmpString );

  // Set the focus to this field so the user can just start typing.
  fldIndex =  FrmGetObjectIndex(frm, fld_winscore);
  FrmSetFocus( frm, fldIndex );

  // Set the handler
  FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  FrmSetFocus( frm, noFocus );

  if ( hitButton == btn_OK_frmNewGame ) {
    Int num;
    
    stor.winscore = DEFAULT_WINSCORE;
    text = FldGetTextPtr( FrmGetObjectPtr (frm, fldIndex) );
    if ( text != NULL ) {
      num = StrAToI( text );
      if( num <= 9999 && num >= 50 ) {
        stor.winscore = num;
      }
    }
    
    stor.openingroll = DEFAULT_OPENINGROLL;
    fldIndex =  FrmGetObjectIndex(frm, fld_openingroll);
    text = FldGetTextPtr( FrmGetObjectPtr (frm, fldIndex) );
    if ( text != NULL ) {
      num = StrAToI( text );
      if( num < stor.winscore && num > 0 ) {
        stor.openingroll = num;
      }
    }
    
    /* Set the number of players/AI */
    stor.numplayers = stor.tmpplayers;
    stor.numcomputers = stor.tmpcomputers;
    stor.total = stor.numplayers + stor.numcomputers;
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

static void ToggleCheck( Word objID, Int flag )
{
  CtlSetValue( GetObjectPtr( objID ),
	       ! (stor.flags & flag) );
  SetFlag( flag, !(stor.flags & flag) );
}
  

static Boolean DialogVariantsHandleEvent (EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
  
  CALLBACK_PROLOGUE

    switch (e->eType) {
    case frmOpenEvent:
	  frm = FrmGetActiveForm();
	  FrmDrawForm(frm);
	  handled = true;
	  break;

    case ctlSelectEvent:
      if( stor.currplayer >= 0 ) {
        /* The player *cannot* continue a
           game once they alter variants */
        ResetCubes(); 
      }
        
      switch(e->data.ctlSelect.controlID) {
      case check_Bump:
        ToggleCheck( check_Bump, flag_Bump );
        if( (stor.flags & flag_Eclipse) &&
            (stor.flags & flag_Bump ) ) {
          ToggleCheck( check_Eclipse, flag_Eclipse );
        }
        break;
      case check_Eclipse:
        ToggleCheck( check_Eclipse, flag_Eclipse );
        if( (stor.flags & flag_Eclipse) &&
            (stor.flags & flag_Bump ) ) {
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
  
  CALLBACK_EPILOGUE
    
    return handled;
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
  CtlSetValue( GetObjectPtr(check_Bump     ), stor.flags & flag_Bump      );
  CtlSetValue( GetObjectPtr(check_Eclipse  ), stor.flags & flag_Eclipse   );
  CtlSetValue( GetObjectPtr(check_Sampler  ), stor.flags & flag_Sampler   );
  CtlSetValue( GetObjectPtr(check_nTW      ), stor.flags & flag_nTW       );
  CtlSetValue( GetObjectPtr(check_Suspend  ), stor.flags & flag_Suspend   );

  // Fill in WinScore with previous value.
  StrIToA( tmpString, stor.nTrainWrecks );
  SetFieldTextFromStr( fldnTrainWrecks, tmpString );

  StrIToA( tmpString, stor.nSuspend );
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
    stor.nTrainWrecks = StrAToI( text );
  } else {
    stor.nTrainWrecks = 3;
  }    

  fldIndex =  FrmGetObjectIndex(frm, fldnSuspend);
  text = FldGetTextPtr( FrmGetObjectPtr( frm, fldIndex ) );
  if( text != NULL ) {
    stor.nSuspend = StrAToI( text );
  } else {
    stor.nSuspend = 10;
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
  Char msg[(MaxName * 2) + 128];
  Word fieldname = 0;

  switch ( frmname ) {
  case frmNextPlayer: 
    fieldname = fldNextPlayer;
    if( stor.total > 1 ) {
      StrPrintF( msg, NextPlayerString,
		 stor.player[p1].name,
		 stor.player[p2].name,
		 NULL);
    } else {
      StrPrintF( msg, "%s", NextSoloPlayerString, NULL );
    }
    break;

  case frmSuspend: 
    if( stor.total == 1 ) {
      return;
    }
    fieldname = fldSuspend;
    StrPrintF( msg, SuspendString,
	       stor.player[p1].name,
	       stor.player[p2].name,
	       NULL);
    break;


  case frmLost:
      fieldname = fldLost;
      StrPrintF( msg, LostString, stor.player[p1].name, NULL );
      break;

  case frmLeader:
    fieldname = fldLeader;
    StrPrintF( msg, LeaderString, stor.player[p1].name, NULL );
    break;

  case frmWinner:
    fieldname = fldWinner;
    StrPrintF( msg, WinnerString, stor.player[p1].name, NULL );
    break;

  case frmNobodyWon:
    fieldname = fldNobodyWon;
    StrPrintF( msg, NobodyWonString, NULL );
    break;

  case frmBump:
    fieldname = fldBump;
    StrPrintF( msg, BumpString,
	       stor.player[p2].name,
	       stor.player[p1].name,
	       stor.player[p2].score,
	       stor.player[p1].score,
	       NULL );
    break;

  case frmSampler:
    fieldname = fldSampler;
    StrPrintF( msg, SamplerString, stor.player[p1].name, NULL );
    break;


  case frmTrainWreck:
    fieldname = fldTrainWreck;
    StrPrintF( msg, TrainWreckString,
	       stor.player[p1].name, 
	       stor.player[p1].TWcount,
	       stor.nTrainWrecks,
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

  if( IsAI( stor.currplayer ) ) {
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

  if( IsAI( stor.currplayer ) ) {
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
			   stor.flags & flag_NextPlayerPopUp );

  // Set the handler
  // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  // Get Controls
  SetFlag( flag_NextPlayerPopUp,
		   CtlGetValue( GetObjectPtr(check_NextPlayerPopUp) ) );

  // Delete the form, we're not using it
  FrmDeleteForm(frm);

  // Restore previous form.
  if (prevForm) {
    FrmSetActiveForm(prevForm);
  }

  // We don't care, as long as the dialog quits.
}


Boolean DialogGetNames() {
  FormPtr prevForm, frm;
  Word hitButton;
  Boolean retVal = false;
  CharPtr text;
  Int i;
  FieldPtr fp;
  VoidPtr vPtr;
  Word oIdx;

  // Save previous form
  prevForm = FrmGetActiveForm();
  // Init new form
  frm = FrmInitForm( frmGetNames);

  // Set it
  FrmSetActiveForm(frm);

  // Set Controls
  for( i = stor.tmpplayers ; i<MaxPlayers ; i++ ) {
    fp = GetObjectPtr( fieldGetNamesPlayer[i] );
    FldSetUsable ( fp, false );
    FldEraseField( fp );
    ShowControl( fieldGetNamesLabel[i], 0 );
  }

  FrmDrawForm(frm);
	  
  for( i=0; i < stor.tmpplayers ; i++ ) {
	SetFieldTextFromStr( fieldGetNamesPlayer[i], stor.player[i].name );
  }
  FrmSetFocus( frm, FrmGetObjectIndex(frm, fieldGetNamesPlayer[0]) );


  // Set the handler
  // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  // Get Controls
  if ( hitButton == btn_OK_frmGetNames ) {
      for( i=stor.tmpplayers + 1;
           i < stor.tmpplayers + stor.tmpcomputers;
           i++ ) {
          StrPrintF( stor.player[i].name, "Robby%d", i );
      }
      for( i=0; i < stor.tmpplayers; i++ ) {
	  oIdx = FrmGetObjectIndex( frm, fieldGetNamesPlayer[i]);
	  vPtr = FrmGetObjectPtr( frm, oIdx );
	  text = FldGetTextPtr( vPtr );
	  StrCopy( stor.player[i].name, text );
          
      }
      
      retVal = true;
  }

  FrmSetFocus( frm, noFocus );

  // Delete the form, we're not using it
  FrmDeleteForm(frm);

  // Restore previous form.
  if (prevForm) {
    FrmSetActiveForm(prevForm);
  }

  return retVal;
}


/* Local Only Functions */
static void SetPlayers  ( Short num );
static void SetComputers( Short num );

#if 0 
static void SetPeople( Short num ) {
  Int x;
  Boolean found = false;
  FormPtr frm;
  ControlPtr cPtr;
  Word oIdx;

  frm = FrmGetActiveForm();

  x = MaxPlayers - 1;

  while( ( x > ( MaxPlayers - (num + 1) ) ) &&
	 !found ) {
    oIdx = FrmGetObjectIndex( frm, pbtnVal[x--] );
    cPtr = FrmGetObjectPtr( frm, oIdx );

    if( CtlGetValue( cPtr ) ) {
      CtlSetValue( cPtr, false );
      found = true;
    }
  }

  if ( found ) {
    stor.tmpcomputers = MaxPlayers - (num + 1);
    oIdx = FrmGetObjectIndex( frm, cbtnVal[stor.tmpcomputers] );
    cPtr = FrmGetObjectPtr( frm, oIdx );
    CtlSetValue( cPtr, true);
  }
}
#endif

  
static void SetPlayers( Short num ) {
  FormPtr frm;
  ControlPtr cPtr;
  Word oIdx;

  /* Bounds checking */
  if( num > MaxPlayers || num < 1 ) {
    ErrNonFatalDisplayIf( true, "SetPlayers: Out of Bounds");
    return;
  }

  /* If it's set, we're done. */
  if( stor.tmpplayers == num ) {
    return;
  }

  frm = FrmGetActiveForm();

  /* Unset the old one */
  oIdx = FrmGetObjectIndex( frm, pbtnVal[stor.tmpplayers - 1] );
  cPtr = FrmGetObjectPtr( frm, oIdx );
  CtlSetValue( cPtr, false );

  /* Set new one */
  oIdx = FrmGetObjectIndex( frm, pbtnVal[num - 1] );
  cPtr = FrmGetObjectPtr( frm, oIdx );
  CtlSetValue( cPtr, true );

  stor.tmpplayers = num;

  if( stor.tmpcomputers + stor.tmpplayers > MaxPlayers ) {
    SetComputers( MaxPlayers - stor.tmpplayers );
  }
  
  return;
}

static void SetComputers( Short num ) {
  FormPtr frm;
  ControlPtr cPtr;
  Word oIdx;

  /* Bounds checking */
  if( num > (MaxPlayers - 1) || num < 0 ) {
    ErrNonFatalDisplayIf( true, "SetComputers: Out of Bounds");
    return;
  }

  /* If it's set, we're done. */
  if( stor.tmpcomputers == num ) {
    return;
  }

  frm = FrmGetActiveForm();

  /* Unset the old one */
  oIdx = FrmGetObjectIndex( frm, cbtnVal[stor.tmpcomputers] );
  cPtr = FrmGetObjectPtr( frm, oIdx );
  CtlSetValue( cPtr, false );

  /* Set new one */
  oIdx = FrmGetObjectIndex( frm, cbtnVal[num] );
  cPtr = FrmGetObjectPtr( frm, oIdx );
  CtlSetValue( cPtr, true );

  stor.tmpcomputers = num;

  if( stor.tmpcomputers + stor.tmpplayers > MaxPlayers ) {
    SetPlayers( MaxPlayers - stor.tmpcomputers );
  }
  
  return;
}

static Boolean DialogNewGameHandleEvent (EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
    
  CALLBACK_PROLOGUE

    switch (e->eType) {
    case frmOpenEvent:
	  frm = FrmGetActiveForm();
	  FrmDrawForm(frm);
	  handled = true;
	  break;

    case ctlSelectEvent:
	  switch(e->data.ctlSelect.controlID) {
		
	  case btn_Variants_frmNewGame:
	    DialogVariants();
	    handled = true;
	    break;

	  case btn_OK_frmNewGame:
	    if( DialogGetNames() == false ) {
	      handled = true;
	      /* So, DGN was canceled, so we eat the result */
	    }
	    break;

          case btn_Default_frmNewGame:
            {
              Char tmpString[5];

              /* Reset Winning Score */
              stor.winscore = DEFAULT_WINSCORE;
              StrIToA( tmpString, stor.winscore );
              SetFieldTextFromStr( fld_winscore, tmpString );

              /* Reset Opening Roll */
              stor.openingroll = DEFAULT_OPENINGROLL;
              StrIToA( tmpString, stor.openingroll );
              SetFieldTextFromStr( fld_openingroll, tmpString );

              /* Reset number of players */
              SetComputers(0);
              SetPlayers(1);
            }
            handled = true;
            break;

	  case cbtn_0:  SetComputers( 0 ); break;
	  case cbtn_1:  SetComputers( 1 ); break;
	  case cbtn_2:  SetComputers( 2 ); break;
	  case cbtn_3:  SetComputers( 3 ); break;
	  case cbtn_4:  SetComputers( 4 ); break;
	  case cbtn_5:  SetComputers( 5 ); break;
	  case cbtn_6:  SetComputers( 6 ); break;
	  case cbtn_7:  SetComputers( 7 ); break;
	  case cbtn_8:  SetComputers( 8 ); break;
	  case cbtn_9:  SetComputers( 9 ); break;

	  case pbtn_1:  SetPlayers( 1 );   break;
	  case pbtn_2:  SetPlayers( 2 );   break;
	  case pbtn_3:  SetPlayers( 3 );   break;
	  case pbtn_4:  SetPlayers( 4 );   break;
	  case pbtn_5:  SetPlayers( 5 );   break;
	  case pbtn_6:  SetPlayers( 6 );   break;
	  case pbtn_7:  SetPlayers( 7 );   break;
	  case pbtn_8:  SetPlayers( 8 );   break;
	  case pbtn_9:  SetPlayers( 9 );   break;
	  case pbtn_10: SetPlayers( 10 );  break;

	  }
	  break;

    default:
	  break;
    }

  CALLBACK_EPILOGUE

    return handled;
}


static VoidPtr GetObjectPtr (Word objID) {
  FormPtr frm;
  
  frm = FrmGetActiveForm();
  
  return (FrmGetObjectPtr (frm, FrmGetObjectIndex (frm, objID)));
}


static void ShowControl(Word objID, Boolean enable) {

  if ( !enable || IsAI( stor.currplayer ) ) {
    CtlHideControl( GetObjectPtr(objID) );
  } else { 
    CtlShowControl( GetObjectPtr(objID) );
  }
  
}

void ShowButtons(Int show) {
  ShowControl(btn_Roll, (Boolean)show);
  if( show && (stor.flags & flag_CanStay) ) {
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

// Local Variables:
// mode:c
// c-basic-offset:2
// End:
