#ifndef _LOWLEVEL_H_
#define _LOWLEVEL_H_

extern void     DrawBitmap (Word BitmapID, int formX, int formY);
extern FieldPtr SetFieldTextFromStr(Word fieldID, CharPtr strP);
extern FieldPtr SetFieldTextFromHandle(Word fieldID, VoidHand txtH);
extern void     ClearFieldText(Word fieldID);

#endif
