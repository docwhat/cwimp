/* $Id$ */

#include <System/SysAll.h>
#include <UI/UIAll.h>

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
 *     Word     fieldID  -- ID of field to fill, see cwimp.rpc file
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
 *     Word     fieldID  -- ID of field to fill, see cwimp.rpc file
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
  ErrNonFatalDisplayIf(!fldP, "missing field");
  oldTxtH = FldGetTextHandle(fldP);

  FldSetTextHandle(fldP, (Handle) txtH);
  FldDrawField(fldP);

  if (oldTxtH)
    MemHandleFree((VoidHand) oldTxtH);

  return fldP;
}


void ClearFieldText(Word fieldID)
{
  SetFieldTextFromHandle(fieldID,NULL);
}

