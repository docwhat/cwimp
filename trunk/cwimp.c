/* $Id$
 * 
 * $Log$
 * Revision 1.1  1999/08/06 05:43:52  palmcvs
 * Initial Revision
 *
 * Revision 1.1  1999/08/06 05:08:07  docwhat
 * Initial revision
 *
 * Revision 0.9  1999/04/11 09:48:25  docwhat
 * Added DialogPreferences
 * Added StatusLine and status help functions
 * flags are now honored in DialogVariants and DialogPreferences
 * preference NextPlayerPopUp works, including popups.
 * Added DialogGetNames to the DialogNewGames.  Names are no
 * longer computer assigned.
 * Cleaned up menus. Added shortcuts for variants and preferences, changed resets.
 *
 * Revision 0.8  1999/03/15 06:19:27  docwhat
 * Added DialogVarients
 * Added MenuItem_Preferences place holder
 * Finished DialogNewGame Handler
 *
 * Revision 0.7  1999/03/14 23:51:36  docwhat
 * Added DialogNewGame.
 * Added hiding Stay button.
 * Added 35 point minimum start.
 *
 * Revision 0.6  1999/03/14 00:55:51  docwhat
 * Seperated out the draw functions
 *
 * Revision 0.5  1999/03/13 23:53:53  docwhat
 * Added ident strings.
 *
 */

#ifndef NOSYSHEADERS
#include <Pilot.h>
//#include <Common.h>
//#include <System/SysAll.h>
//#include <UI/UIAll.h>
#endif

#include "callback.h"
#include "statusmsg.h"
#include "game.h"
#include "draw.h"
#include "cwroller.h"

static Boolean MainFormHandleEvent (EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
    
  CALLBACK_PROLOGUE

    switch (e->eType) {
    case frmOpenEvent:
	  frm = FrmGetActiveForm();
	  FrmDrawForm(frm);
	  DrawState();  // Draw those dice!
	  handled = true;
	  break;

    case menuEvent:
	  MenuEraseStatus(NULL);

	  switch(e->data.menu.itemID) {
	  case MenuItem_About:
		FrmAlert(About_Info);
		break;

	  case MenuItem_New:
		DialogNewGame();
		break;

	  case MenuItem_Reset:
		ResetCubes();
		DrawState();
		break;

	  case MenuItem_Variants:
		if ( stor.currplayer < 0 ) {
		  DialogVarients();
		} else {
		  FrmCustomAlert( calertDEBUG,
						  "Can't do variants during game", 
						  "ToDo: do this properly",
						  " ");
		}
		break;

	  case MenuItem_Preferences:
		DialogPreferences();
		break;


	  }

	  handled = true;
	  break;

    case ctlSelectEvent:
	  switch(e->data.ctlSelect.controlID) {
		
	  case btn_Stay:
		Stay();
		break;

	  case btn_Info:
		//FrmAlert(About_Info);
		DialogStatus();
		break;
		
	  case btn_Roll:
		Roll();
		break;
	  }
	  handled = true;
	  break;

    default:
	  break;
    }

  CALLBACK_EPILOGUE

    return handled;
}

static Boolean ApplicationHandleEvent(EventPtr e)
{
  FormPtr frm;
  Word    formId;
  Boolean handled = false;

  if (e->eType == frmLoadEvent) {
	formId = e->data.frmLoad.formID;
	frm = FrmInitForm(formId);
	FrmSetActiveForm(frm);

	switch(formId) {
	case MainForm:
	  FrmSetEventHandler(frm, MainFormHandleEvent);
	  break;
	}
	handled = true;
  }

  return handled;
}

/* Get preferences, open (or create) app database */
static Word StartApplication(void)
{
  FrmGotoForm(MainForm);
//  ResetCubes(); // Eventually, we'll try to recover them from preferences
  LoadCubes();
  return 0;
}

/* Save preferences, close forms, close app database */
static void StopApplication(void)
{
  SaveCubes();
  FrmSaveAllForms();
  FrmCloseAllForms();
}

/* The main event loop */
static void EventLoop(void)
{
  Word err;
  EventType e;

  do {
	EvtGetEvent(&e, evtWaitForever);
	if (! SysHandleEvent (&e))
	  if (! MenuHandleEvent (NULL, &e, &err))
		if (! ApplicationHandleEvent (&e))
		  FrmDispatchEvent (&e);
  } while (e.eType != appStopEvent);
}

/* Main entry point; it is unlikely you will need to change this except to
   handle other launch command codes */
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
  if (cmd == sysAppLaunchCmdNormalLaunch) {
    Word err;


	err = StartApplication();
	if (err) return err;
	
	EventLoop();
	StopApplication();
	
  } else {
	return sysErrParamErr;
  }

return 0;
}
