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

#include "queue.h"
#include "draw.h"

/* Pointer to the Head of the EventQueue */
EQPtr EQueue;

static 
EQPtr
newEvent(void (*func)(Int), Int data) {
  EQPtr new;

  new = MemPtrNew(sizeof(EQ));
  ErrNonFatalDisplayIf(new == NULL, "Queue:new failed to get memory");
  new->next = NULL;
  new->func = func;
  new->data = data;

  return new;
}

inline
void EQInit(void) {
  EQueue = newEvent(NULL,0);
}
  
void EQAdd(void (*func)(Int),Int data) {
  EQPtr ptr;
  
  ptr = EQueue;
  while( ptr->next != NULL ) {
    ptr = ptr->next;
  }
  
  ptr->next = newEvent( func, data );
}

Boolean EQRunNext(void) {
  EQPtr ptr;
  void (*func)(Int);
  Int data;

  ptr = EQueue->next;

  if( EQIsEmpty() ) return false;

  func = ptr->func;
  data = ptr->data;
  EQueue->next = ptr->next;

  ErrNonFatalDisplayIf( MemPtrFree( ptr ),
                        "Queue:RunNext failed to free memory.");
  func(data);
  return true;
}

void EQReset(void)
{
  EQPtr ptr;
  EQPtr top;

  if( EQIsEmpty() ) return;

  top = EQueue->next;

  for( ptr = top;
       ptr != NULL;
       ptr = top ) {
    top = top->next;
    ErrNonFatalDisplayIf( MemPtrFree( ptr ),
                          "Queue:RunNext failed to free memory.");
  }

  ErrNonFatalDisplayIf( top != NULL,
                        "Queue:RunNext EQueue isn't empty.");

  EQueue->next = top;
}

Boolean EQIsEmpty(void)
{
  if( EQueue->next == NULL ) return true;
  return false;
}


void EQNOP(Int x)
{
  /* Does Nothing! */
  return;
}
