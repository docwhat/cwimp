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
/* This contains all the status messages that might need to be displayed.
 * Hopefully, translation will be easily done by switching this file and the
 * .rcp file with the correct language's version.
 */

#include <Pilot.h>
#include <System/SysAll.h>
#include <UI/UIAll.h>

#include "game.h"
#include "statusmsg.h"
#include "lowlevel.h"

void DrawStatus() {

  switch (stor.status) {
  case DS_MustClearFlash:
	SetFieldTextFromStr( statusLine, "Must Clear Flash" );
	break;
  case DS_YMNWTBYM: 
	SetFieldTextFromStr( statusLine, "Y.M.N.W.T.B.Y.M." );
	break;
  case DS_NextPlayer:
	SetFieldTextFromStr( statusLine, "Next Player's Turn" );
	break;
  default: ClearFieldText( statusLine ); break;
  }

}

void DialogStatus() {

  switch (stor.status) {
  case DS_MustClearFlash:
	FrmCustomAlert( calertSTATUS,
					"In a previous turn you rolled a flash (a three of a kind). In order to clear this flash and continue with your turn, all the dice of your next roll must be different from the flash.  If any dice match the flash, then that roll doesn't count and you will have to roll again.",
					" ", " " );
	break;
  case DS_YMNWTBYM:
	FrmCustomAlert( calertSTATUS,
					"You May Not Want To But You Must:\n\tYou have scored with all the dice and are not allowed to \"stay\" without rolling at least one more time.",
					" ", " " );
	break;
  case DS_NextPlayer:
	FrmCustomAlert( calertSTATUS,
					"Your are at the next player's turn, no dice are in use for scoring, and you have no points this turn (and roll) yet.\nYou can't 'stay' without rolling once.",
					" ", " " ); 
	break;
  default: break;
  }

}


