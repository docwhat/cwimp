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

#include "callback.h"
#include "cwimp.h"
#include "lowlevel.h"
#include "game.h"
#include "statusmsg.h"
#include "statusmsgstrings.h"

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

const UInt bmpWCube[6] = {
  bmp10w,  
  bmp2w,     
  bmp3w,   
  bmp4w,   
  bmp5w,  
  bmp6w
};   
const UInt bmpBCube[6] = {
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



void DrawState()
{
  Short x;
  Char msg[MaxName+4];

  DrawCurrScore();

  // Fill the fields
  x = 0;
  if ( stor.currplayer > -1 ) {
	// If there is a game on, fill in the names and scores
	for ( ; x < stor.numplayers ; x++ ) {
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


void DrawKeepBit(Byte die)
{
  
  if (stor.cube[die].keep && stor.cube[die].value != 0) {
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
	StrCopy( msg, "-N/A-" );
  } else {
	StrIToA( msg, stor.scorethisroll );
  }
  SetFieldTextFromStr(scoreRoll,  msg);

  StrIToA( msg, stor.scorethisturn );
  SetFieldTextFromStr(scoreTurn,  msg);

  StrIToA( msg, stor.currscore );
  SetFieldTextFromStr(scoreTot,  msg);

}

/* DrawCube -- Draws the cube at the correct place
 * Args:    
 *     Byte  die   -- Cube to be rolled and drawn.
 * Returns: None
 */
void DrawCube(Byte die)
{
  int tmpID;
  int y;

  y = CubesTop + (CubeSize + CubeShift) * die;

  // Select correct Bitmap
  if ( die == 0 ) { 
	tmpID = bmpBCube[ stor.cube[die].value - 1 ];
  } else {
	tmpID = bmpWCube[ stor.cube[die].value - 1 ];
  }

  if ( stor.cube[die].value == 0 ) { 
	tmpID = bmpBlank;
  } else {
	DrawBitmap( bmpBlank, CubesLeft, y );  // Put up the blank tile
#if 0
	SysTaskDelay( 0.15*sysTicksPerSecond); // Delay for cinematic effect
#endif
  }

  DrawBitmap( tmpID, CubesLeft, y );
      
}

void DrawRollButton() {
  if( stor.currplayer < 0 ) {
    CtlSetLabel( GetObjectPtr(btn_Roll), StartString );
  } else {
    CtlSetLabel( GetObjectPtr(btn_Roll), RollString );
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

  /* No game, no brainer */
  if( stor.currplayer < 0 ) {
    ShowControl( btn_Stay, 0 );
    return;
  }

  if( stor.flash ) {
    ShowControl( btn_Stay, 0 );
    SetStatus( DS_MustClearFlash );
    return;
  }

  if( stor.YMNWTBYM ) {
    ShowControl( btn_Stay, 0 );
    SetStatus( DS_YMNWTBYM );
    return;
  }

  if( stor.scorethisturn == 0 ) {
    ShowControl( btn_Stay, 0 );
    SetStatus( 0 );
    return;
  }
    
  if( stor.player[stor.currplayer].score == 0 &&
      stor.scorethisturn < stor.openingroll )
    {
      ShowControl( btn_Stay, 0 );
      return;
    }

  /* If we aren't winning in last licks */
  if( stor.leader >= 0 &&
      ( stor.player[stor.leader].score > stor.currscore ) )
    {
      ShowControl( btn_Stay, 0 );
      return;
    }

  if( stor.flags & flag_Eclipse ) {
    /* Are we eclipsed? */
    for( x = 0; x < stor.numplayers; x++ ) {
      if( x == stor.currplayer ) continue;
      if( stor.player[x].score == stor.currscore ) break;
    }
    
    if( x != stor.numplayers ) {
      ShowControl( btn_Stay, 0 );
      SetStatus( DS_Eclipse );
      return;
    }
  }

  /* Otherwise, show it */
  ShowControl( btn_Stay, 1 );
  SetStatus( 0 );
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
	stor.winscore = 300;
  }
  StrIToA( tmpString, stor.winscore );
  SetFieldTextFromStr( fld_winscore, tmpString );
  // Set the focus to this field so the user can just start typing.
  fldIndex =  FrmGetObjectIndex(frm, fld_winscore);
  FrmSetFocus( frm, fldIndex );

  // Set the handler
  FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  if ( hitButton == btn_OK_frmNewGame ) {
	text = FldGetTextPtr( FrmGetObjectPtr (frm, fldIndex) );
	if ( text != NULL ) {
  	  stor.winscore = StrAToI( text );
	} else {
	  stor.winscore = 300;
	}

	stor.numplayers = stor.tmpplayers;
	stor.numcomputers = stor.tmpcomputers;
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
	  case check_FullHouse:
	    ToggleCheck(check_FullHouse, flag_FullHouse );
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
  CtlSetValue( GetObjectPtr(check_FullHouse), stor.flags & flag_FullHouse );
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
    if( (stor.numplayers + stor.numcomputers) > 1 ) {
      StrPrintF( msg, NextPlayerString,
		 stor.player[p1].name,
		 stor.player[p2].name,
		 NULL);
    } else {
      StrPrintF( msg, "%s", NextSoloPlayerString, NULL );
    }
    break;

  case frmSuspend: 
    if( (stor.numplayers + stor.numcomputers) == 1 ) {
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


Int DialogChooseTwo( CharPtr fText, CharPtr bOne, CharPtr bTwo ) {
  FormPtr prevForm, frm;
  Word hitButton;
  
  // Save previous form
  prevForm = FrmGetActiveForm();
  // Init new form
  frm = FrmInitForm( frmChooseTwo );

  // Set it
  FrmSetActiveForm(frm);
  FrmDrawForm(frm);

  // Fill it...
  CtlSetLabel( GetObjectPtr(btnChooseTwo1), bOne );
  CtlSetLabel( GetObjectPtr(btnChooseTwo2), bTwo );
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

  if( hitButton == btnChooseTwo1 ) return 1;
  if( hitButton == btnChooseTwo2 ) return 2;
  return ( -1 );
}


Int DialogChooseThree( CharPtr fText,
			CharPtr bOne, CharPtr bTwo, CharPtr bThree ) {
  FormPtr prevForm, frm;
  Word hitButton;
  
  // Save previous form
  prevForm = FrmGetActiveForm();
  // Init new form
  frm = FrmInitForm( frmChooseThree );

  // Set it
  FrmSetActiveForm(frm);
  FrmDrawForm(frm);

  // Fill it...
  CtlSetLabel( GetObjectPtr(btnChooseThree1), bOne );
  CtlSetLabel( GetObjectPtr(btnChooseThree2), bTwo );
  CtlSetLabel( GetObjectPtr(btnChooseThree3), bThree );
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

  if( hitButton == btnChooseThree1 ) return 1;
  if( hitButton == btnChooseThree2 ) return 2;
  if( hitButton == btnChooseThree3 ) return 3;
  return ( -1 );
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
	
	for( i=0 ; i<stor.tmpplayers ; i++ ) {
	  oIdx = FrmGetObjectIndex( frm, fieldGetNamesPlayer[i]);
	  vPtr = FrmGetObjectPtr( frm, oIdx );
	  text = FldGetTextPtr( vPtr );
	  StrCopy( stor.player[i].name, text );

	}

	retVal = true;
  }


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

  if( num > MaxPlayers || num < 1 ) {
    ErrNonFatalDisplayIf( true, "SetPlayers: Out of Bounds");
    return;
  }

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

  if( num > (MaxPlayers - 1) || num < 0 ) {
    ErrNonFatalDisplayIf( true, "SetComputers: Out of Bounds");
    return;
  }

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

#if 0
	  case cbtn_0: SetComputers( 0 ); break;
	  case cbtn_1: SetComputers( 1 ); break;
	  case cbtn_2: SetComputers( 2 ); break;
	  case cbtn_3: SetComputers( 3 ); break;
	  case cbtn_4: SetComputers( 4 ); break;
	  case cbtn_5: SetComputers( 5 ); break;
	  case cbtn_6: SetComputers( 6 ); break;
	  case cbtn_7: SetComputers( 7 ); break;
	  case cbtn_8: SetComputers( 8 ); break;
	  case cbtn_9: SetComputers( 9 ); break;
#else
	  case cbtn_0: 
	  case cbtn_1: 
	  case cbtn_2: 
	  case cbtn_3: 
	  case cbtn_4: 
	  case cbtn_5: 
	  case cbtn_6: 
	  case cbtn_7: 
	  case cbtn_8: 
	  case cbtn_9: SetComputers( 0 ); break;
#endif
	  case pbtn_1: SetPlayers( 1 ); break;
	  case pbtn_2: SetPlayers( 2 ); break;
	  case pbtn_3: SetPlayers( 3 ); break;
	  case pbtn_4: SetPlayers( 4 ); break;
	  case pbtn_5: SetPlayers( 5 ); break;
	  case pbtn_6: SetPlayers( 6 ); break;
	  case pbtn_7: SetPlayers( 7 ); break;
	  case pbtn_8: SetPlayers( 8 ); break;
	  case pbtn_9: SetPlayers( 9 ); break;
	  case pbtn_10: SetPlayers( 10 ); break;

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

  if ( enable ) {
	CtlShowControl( GetObjectPtr(objID) );
  } else { 
	CtlHideControl( GetObjectPtr(objID) );
  }

}
