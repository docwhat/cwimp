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

#include "game.h"
#include "draw.h"

void CheckAI(void)
{
  Boolean isai;
  isai = IsAI( stor.currplayer );

  SetFlag( flag_PendingAI, isai );

  EnableControl( btn_Roll, !isai );
  EnableControl( btn_Stay, !isai );

}


void AITurn(void)
{
  FrmCustomAlert( calertDEBUG, "Pre-AITurn", " ", " " );

  if( stor.flags & flag_CanStay ) {
    Stay();
  } else {
    Roll();
  }

  return;
}
