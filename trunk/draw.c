/* $Id$

    CWimp - Dice game for the palm handhelds.
    Copyright (C) 2000 Christian H�ltje

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
  pbtn_01,
  pbtn_02,
  pbtn_03,
  pbtn_04,
  pbtn_05,
  pbtn_06,
  pbtn_07,
  pbtn_08,
  pbtn_09,
  pbtn_10
};
const UInt cbtnVal[10] = {
  cbtn_00,
  cbtn_01,
  cbtn_02,
  cbtn_03,
  cbtn_04,
  cbtn_05,
  cbtn_06,
  cbtn_07,
  cbtn_08,
  cbtn_09
};



void DrawState()
{
  Byte x;
  Char msg[MaxName+4];

  DrawCurrScore();

  // Fill the fields
  x = 0;
  if ( stor.currplayer > -1 ) {
	// If there is a game on, fill in the names and scores
	for ( ; x < stor.numplayers ; x++ ) {
	  StrPrintF( msg, "%d. %s", x+1, stor.player[x].name ); 
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
  DrawStatus();
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


void DrawPlayerScore(Byte player) {
  Char msg[MaxName];

  StrIToA( msg, stor.player[player].score );
  SetFieldTextFromStr( fieldScorePlayer[player], msg );

  // ToDo: Add marker for players out of the game
  if ( player == stor.currplayer ) {
	SetFieldTextFromStr( fieldMarkPlayer[player], ArrowSymbol );
  } else {
	ClearFieldText( fieldMarkPlayer[player] );
	// SetFieldTextFromStr( fieldMarkPlayer[player], BlankSymbol );
  }
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
	SysTaskDelay( 0.15*sysTicksPerSecond); // Delay for cinematic effect
  }

  DrawBitmap( tmpID, CubesLeft, y );
      
}

void DrawStayButton() {

  if ( stor.flash ||
	   stor.YMNWTBYM ||
	   ( stor.player[stor.currplayer].score == 0 &&
		 stor.scorethisturn < stor.openingroll ) ||
	   stor.scorethisturn == 0
	   ) {
	ShowControl( btn_Stay, 0 );
  } else {
	ShowControl( btn_Stay, 1 );
  }
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

  // Set Controls
  // cbtnVal & pbtnVal
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
	
	for ( x = 0 ; x < 10 ; x++ ) {
	  if ( CtlGetValue( GetObjectPtr( pbtnVal[x] ) ) ) {
		stor.numplayers = x+1;
	  } else if ( CtlGetValue( GetObjectPtr( cbtnVal[x] ) ) ) {
		stor.numcomputers = x;
	  }
	}
  } // if hitButton == ...

  // ToDo: I think I'm supposed to unlock the Ptr from fldIndex

  /* Manually erase the field.  It doesn't do it automatically
   * for some reason.
   */
  FldEraseField( GetObjectPtr( fld_winscore) );

  // Restore previous form.
  if (prevForm) {
	FrmSetActiveForm(prevForm);

	// FrmDrawForm(prevForm);
	/* We don't need this since DialogGetNames will do it
	 * for us. */
  }

  // Delete the form, we're not using it
  FrmDeleteForm(frm);


  if ( hitButton == btn_OK_frmNewGame ) {
	if( DialogGetNames() ) {
	  NewGame();
	}
  }


  DrawState();
}


void DialogVarients() {
  FormPtr prevForm, frm;
  Word hitButton;
  
  // Save previous form
  prevForm = FrmGetActiveForm();
  // Init new form
  frm = FrmInitForm( frmVariants );

  // Set it
  FrmSetActiveForm(frm);

  // Set Controls
  CtlSetValue( GetObjectPtr(check_Bump     ), stor.flags & flag_Bump      );
  CtlSetValue( GetObjectPtr(check_Eclipse  ), stor.flags & flag_Eclipse   );
  CtlSetValue( GetObjectPtr(check_Sampler  ), stor.flags & flag_Sampler   );
  CtlSetValue( GetObjectPtr(check_nTW      ), stor.flags & flag_nTW       );
  CtlSetValue( GetObjectPtr(check_FullHouse), stor.flags & flag_FullHouse );
  CtlSetValue( GetObjectPtr(check_Suspend  ), stor.flags & flag_Suspend   );
  CtlSetValue( GetObjectPtr(check_Insurance), stor.flags & flag_Insurance );

  // Set the handler
  // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  // Get Controls
  SetFlag( flag_Bump,
		   CtlGetValue( GetObjectPtr(check_Bump) ) );
  SetFlag( flag_Eclipse,
		   CtlGetValue( GetObjectPtr(check_Eclipse) ) );
  SetFlag( flag_Sampler,
		   CtlGetValue( GetObjectPtr(check_Sampler) ) );
  SetFlag( flag_nTW,
		   CtlGetValue( GetObjectPtr(check_nTW) ) );
  SetFlag( flag_FullHouse,
		   CtlGetValue( GetObjectPtr(check_FullHouse) ) );
  SetFlag( flag_Suspend,
		   CtlGetValue( GetObjectPtr(check_Suspend) ) );
  SetFlag( flag_Insurance,
		   CtlGetValue( GetObjectPtr(check_Insurance) ) );

  // Restore previous form.
  if (prevForm) {
	FrmSetActiveForm(prevForm);
	FrmDrawForm(prevForm);
  }

  // Delete the form, we're not using it
  FrmDeleteForm(frm);

  // We don't care, as long as the dialog quits.
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

  // Set Controls
  CtlSetValue( GetObjectPtr(check_NextPlayerPopUp ),
			   stor.flags & flag_NextPlayerPopUp );

  // Set the handler
  // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  // Get Controls
  SetFlag( flag_NextPlayerPopUp,
		   CtlGetValue( GetObjectPtr(check_NextPlayerPopUp) ) );

  // Restore previous form.
  if (prevForm) {
	FrmSetActiveForm(prevForm);
	FrmDrawForm(prevForm);
  }

  // Delete the form, we're not using it
  FrmDeleteForm(frm);

  // We don't care, as long as the dialog quits.
}


Boolean DialogGetNames() {
  FormPtr prevForm, frm;
  Word hitButton;
  Boolean retVal;
  CharPtr text;
  Int i;
  FieldPtr fp;

  // Save previous form
  prevForm = FrmGetActiveForm();
  // Init new form
  frm = FrmInitForm( frmGetNames);

  // Set it
  FrmSetActiveForm(frm);

  // Set Controls
  for( i=0; i<stor.numplayers ; i++ ) {
	//	SetFieldTextFromStr( fieldGetNamesPlayer[i], stor.player[i].name );
	SetFieldTextFromStr( fieldGetNamesPlayer[i], stor.player[i].name );
  }

  for( ; i<MaxPlayers ; i++ ) {
	fp = GetObjectPtr( fieldGetNamesPlayer[i] );
	FldSetUsable ( fp, false );
	FldEraseField( fp );
	ShowControl( fieldGetNamesLabel[i], 0 );
  }
  FrmSetFocus( frm, FrmGetObjectIndex(frm, fieldGetNamesPlayer[0]) );
	  

  // Set the handler
  // FrmSetEventHandler(frm, DialogNewGameHandleEvent);

  hitButton = FrmDoDialog(frm);

  retVal = false;
  // Get Controls
  if ( hitButton == btn_OK_frmGetNames ) {
	
	for( i=0 ; i<stor.numplayers ; i++ ) {
	  text =  FldGetTextPtr( FrmGetObjectPtr( frm,
											  FrmGetObjectIndex(frm,
																fieldGetNamesPlayer[i]) ) );
	  StrCopy( stor.player[i].name, text );
					 
	  // For some reason, the palm doesn't erase these for us.
	  FldEraseField( GetObjectPtr( fieldGetNamesPlayer[i] ) );
	}

	retVal = true;
  }

  // Restore previous form.
  if (prevForm) {
	FrmSetActiveForm(prevForm);
	FrmDrawForm(prevForm);
  }

  // Delete the form, we're not using it
  FrmDeleteForm(frm);

  return retVal;
}


/* Local Only Functions */
static void SetPeople   ( Short computersetting );
static void SetComputer ( Short peoplesetting );

static void SetPeople   ( Short computersetting ) {
  Int x;
  Boolean found;
  FormPtr frm;
  ControlPtr cPtr;

  found = false;
  frm = FrmGetActiveForm();
  
  for ( x = MaxPlayers - 1 ;
		x > MaxPlayers - (computersetting + 1)  ;
		x-- ) {
	cPtr = FrmGetObjectPtr (frm, FrmGetObjectIndex (frm, pbtnVal[x] ));
	if ( CtlGetValue( cPtr ) ) {
	  CtlSetValue( cPtr, false );
	  found = true;
	}
  }
  if ( found ) {
	CtlSetValue( FrmGetObjectPtr (frm, 
								  FrmGetObjectIndex (frm,
													 pbtnVal[ MaxPlayers -
															(computersetting+1) ] )),
				 true );
  }
}
  
static void SetComputer ( Short peoplesetting ) {
  Int x;
  Boolean found;
  FormPtr frm;
  ControlPtr cPtr;

  found = false;
  frm = FrmGetActiveForm();
  
  for ( x = MaxPlayers - 1 ;
		x > MaxPlayers - peoplesetting;
		x-- ) {
	cPtr = FrmGetObjectPtr (frm, FrmGetObjectIndex (frm, cbtnVal[x] ));
	if ( CtlGetValue( cPtr ) ) {
	  CtlSetValue( cPtr, false );
	  found = true;
	}
  }
  if ( found ) {
	CtlSetValue( FrmGetObjectPtr (frm, 
								  FrmGetObjectIndex (frm,
													 cbtnVal[ MaxPlayers -
															peoplesetting ] )),
				 true );
  }
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
		DialogVarients();
		handled = true;
		break;

	  case pbtn_02: SetComputer( 2 ); break;
	  case pbtn_03: SetComputer( 3 ); break;
	  case pbtn_04: SetComputer( 4 ); break;
	  case pbtn_05: SetComputer( 5 ); break;
	  case pbtn_06: SetComputer( 6 ); break;
	  case pbtn_07: SetComputer( 7 ); break;
	  case pbtn_08: SetComputer( 8 ); break;
	  case pbtn_09: SetComputer( 9 ); break;
	  case pbtn_10: SetComputer( 10 ); break;

	  case cbtn_01: SetPeople( 1 ); break;
	  case cbtn_02: SetPeople( 2 ); break;
	  case cbtn_03: SetPeople( 3 ); break;
	  case cbtn_04: SetPeople( 4 ); break;
	  case cbtn_05: SetPeople( 5 ); break;
	  case cbtn_06: SetPeople( 6 ); break;
	  case cbtn_07: SetPeople( 7 ); break;
	  case cbtn_08: SetPeople( 8 ); break;
	  case cbtn_09: SetPeople( 9 ); break;

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
