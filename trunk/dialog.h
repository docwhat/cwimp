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
#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "game.h"

void DialogNewGame();
void DialogVariants();
void DialogOK( Word frm, Short player1, Short player2 );
Int DialogChooseTwo( CharPtr fText, Int c1, Int c2 );
Int DialogChooseThree( CharPtr fText, Int c1, Int c2, Int c3 );
void DialogPreferences();
Boolean DialogGetNames();

#endif
