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
#include "lowlevel.h"

/* DrawBitmap -- Places a bitmap at a specified location
 * Args:    
 *     Word  BitmapID  -- ID for bitmap, see the cwimp.rpc file
 *     int   formX     -- X location on the form
 *     int   formY     -- Y location on the form
 * Returns: None
 */
void DrawBitmap (Word BitmapID, int formX, int formY)
{
  VoidHand resourceHandle;
  BitmapPtr bitmapPntr;

  resourceHandle = DmGetResource(bitmapRsc, BitmapID);
  bitmapPntr = MemHandleLock(resourceHandle);
  WinDrawBitmap(bitmapPntr, formX, formY);
  MemPtrUnlock(bitmapPntr);
  DmReleaseResource(resourceHandle);
}

	
/* SetFieldTextFromStr -- Fills a field with a string
 * Args:    
 *     Word     fieldID  -- ID of field to fill, see cwimp.rcp file
 *     CharPtr  strP     -- String to fill ID with
 * Returns:
 *     FieldPtr          -- Ptr to the field set.
 */
FieldPtr SetFieldTextFromStr(Word fieldID, CharPtr strP)
{
  VoidHand txtH;
  
  txtH = MemHandleNew(StrLen(strP) + 1);
  if(!txtH) return NULL;
  
  StrCopy(MemHandleLock(txtH), strP);
  
  // ToDo: SetFieldTextFromHandle should happen *before* unlock
  MemHandleUnlock(txtH);
  
  return SetFieldTextFromHandle(fieldID, txtH);
}



/* SetFieldTextFromHandle -- Fills a field with contents of a handle
 * Args:    
 *     Word     fieldID  -- ID of field to fill, see cwimp.rcp file
 *     VoidHand txtH     -- Handle of String to fill ID with
 * Returns:
 *     FieldPtr          -- Ptr to the field set.
 */
FieldPtr SetFieldTextFromHandle(Word fieldID, VoidHand txtH)
{
  Handle   oldTxtH;
  FormPtr  frm = FrmGetActiveForm();
  FieldPtr fldP;

  fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fieldID));
  ErrNonFatalDisplayIf(fldP == NULL, "missing field");
  oldTxtH = FldGetTextHandle(fldP);

  FldSetTextHandle(fldP, (Handle) txtH);
  // FldEraseField( fldP );
  FldDrawField(fldP);

  if (oldTxtH)
    MemHandleFree((VoidHand) oldTxtH);

  return fldP;
}


VoidPtr GetObjectPtr (Word objID) {
  FormPtr frm;
  
  frm = FrmGetActiveForm();
  
  return (FrmGetObjectPtr (frm, FrmGetObjectIndex (frm, objID)));
}



void ClearFieldText(Word fieldID)
{
  SetFieldTextFromHandle(fieldID,NULL);
}

