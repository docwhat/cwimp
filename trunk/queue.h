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
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <PalmOS.h>
#include "data.h"

struct EQueueStruct {
  void (*func)(DieType);
  DieType data;
  struct EQueueStruct *next;
};

typedef struct EQueueStruct EQ;
typedef EQ* EQPtr;

extern EQPtr EQueue;

void	EQInit(void);
void	EQAdd(void (*func)(DieType), DieType data);
Boolean EQRunNext(void);
Boolean EQIsEmpty(void);
void	EQDrain(void);
void	EQReset(void);
void	EQNOP(DieType x);

#endif
