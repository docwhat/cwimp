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

#ifndef _LOWLEVEL_H_
#define _LOWLEVEL_H_

void     DrawBitmap (UInt16 BitmapID, UInt16 formX, UInt16 formY);
FieldPtr SetFieldTextFromStr(UInt16 fieldID, Char *strP);
FieldPtr SetFieldTextFromHandle(UInt16 fieldID, MemHandle txtH);

void*	GetObjectPtr(UInt16 objID);
void	SetFocus(UInt16 objID);
void	ClearFieldText(UInt16 fieldID);

#endif
