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

/* Pointer to the Head of the EventQueue */
EQPtr EQueue;

static 
EQPtr
newEvent(VoidPtr func) {
  EQPtr new;

  new = MemPtrNew(sizeof(EQ));
  new->next = NULL;
  new->func = func;

  return new;
}

inline
void EQInit(void) {
  EQueue = newEvent(NULL);
}
  
void EQAdd(VoidPtr func) {
  EQPtr ptr;
  
  ptr = EQueue;
  while( ptr->next != NULL ) {
    ptr = ptr->next;
  }
  
  ptr->next = newEvent( func );
}

VoidPtr EQTop(void) {
  EQPtr ptr;
  VoidPtr func;

  ptr = EQueue->next;
  func = ptr->func;
  EQueue->next = ptr->next;

  MemPtrFree( ptr );
  return func;
}
