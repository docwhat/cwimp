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
#ifndef __CALLBACK_H__
#define __CALLBACK_H__

/* This is a workaround for a bug in the current version of gcc:

   gcc assumes that no one will touch %a4 after it is set up in crt0.o.
   This isn't true if a function is called as a callback by something
   that wasn't compiled by gcc (like FrmCloseAllForms()).  It may also
   not be true if it is used as a callback by something in a different
   shared library.

   We really want a function attribute "callback" which will insert this
   progloue and epilogoue automatically.
   
      - Ian */

register void *reg_a4 asm("%a4");

#define CALLBACK_PROLOGUE \
    void *save_a4 = reg_a4; asm("move.l %%a5,%%a4; sub.l #edata,%%a4" : :);

#define CALLBACK_EPILOGUE reg_a4 = save_a4;

#endif
