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

#include "autogen.h"
#include "game.h"
#include "draw.h"
#include "data.h"

void CheckAI(void)
{
  Boolean isai;
  isai = IsAI( pref.currplayer );

  SetFlag( flag_PendingAI, isai );
  if( isai ) SetStatus( DS_Thinking );
}


void AITurn(void)
{
  /* If we have no choice... */
  if( ! GetFlag(flag_CanStay) ) {
    Roll();
    return;
  }

  /* This is where the AI should come into play */
  Stay();

  return;
}

Int AIChooseTwo( Int c1, Int c2 )
{
  /* This doesn't matter much, unless it is a choice between 10 and 5 */
  if( c2 == 1 )
    return c2;

  return c1;
}

Int AIChooseThree( Int c1, Int c2, Int c3 )
{
  Short x, count;

  count = 0;
  for( x = 0 ; x < NumCubes ; x++ ) {
    if( pref.cube[x].keep )
      count++;
  }

  if( c3 == 0 && count > 2 ) {
    return c3;
  }

  /* If they both are scoring numbers, pick
     the higher number! */
  if( (c1 == 1 && c2 == 5) ||
      (c1 == 5 && c2 == 1) )
    return 1;

  return c1;
}
