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
#include "callback.h"
#include "statusmsg.h"
#include "game.h"
#include "draw.h"
#include "cwimp.h"
#include "queue.h"

static Boolean MainFormHandleEvent (EventPtr e)
{
  Boolean handled = false;
  FormPtr frm;
    
  CALLBACK_PROLOGUE

    switch (e->eType) {
    case frmOpenEvent:
      frm = FrmGetActiveForm();
      DrawIntro();
      FrmDrawForm(frm);
      DrawState();  // Draw those dice!
      handled = true;
      break;

    case menuEvent:
      MenuEraseStatus(NULL);

      switch(e->data.menu.itemID) {
      case MenuItem_Help:
	FrmHelp( GenericHelp );
	break;

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

      case MenuItem_Defaults:
	Defaults();
	ResetCubes();
	DrawState();
	break;

      case MenuItem_Variants:
	if ( stor.currplayer < 0 ) {
	  DialogVariants();
	} else {
	  FrmAlert(NoVariants);
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
        ShowButtons(false);
        Stay();
	break;

      case btn_Info:
	//FrmAlert(About_Info);
	DialogStatus();
	break;
		
      case btn_Roll:
	if ( stor.currplayer < 0 ) {
	  DialogNewGame();
	} else {
          ShowButtons(false);
          Roll();
	}
	break;

      case btnTopStatus:
	FrmHelp( TS_LastLicks );
	break;

      }
      handled = true;
      break;

    case winExitEvent:
      if( e->data.winExit.exitWindow == (WinHandle)FrmGetFormPtr(MainForm)) {
        /* Turn off animations and the AI */
        FreezeBit = true;
      }
      break;

    case winEnterEvent:
      if (e->data.winEnter.enterWindow == (WinHandle)FrmGetFormPtr(MainForm) &&
          e->data.winEnter.enterWindow == (WinHandle)FrmGetFirstForm ()) {
        /* Turn back on animations and the AI */
        FreezeBit = false;
      }

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
  DWord romversion;
  Int err;

  err = FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romversion);

  /* We *must* be version 2 or greater */
  if( err || (romversion < 0x02003000) ) {
    FrmAlert(alertVersion);
    return(1);
  }

  FrmGotoForm(MainForm);
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
    if( EQIsEmpty() ) {
      EvtGetEvent(&e, .66 * SysTicksPerSecond());
    } else {
      EvtGetEvent(&e, .05 * SysTicksPerSecond());
    }
    if (! SysHandleEvent (&e))
      if (! MenuHandleEvent (NULL, &e, &err) )
	if (! ApplicationHandleEvent (&e) )
	  if( ! FrmDispatchEvent (&e) )
            GameEvents();
  } while (e.eType != appStopEvent);
}

/* Main entry point; it is unlikely you will need to change this except to
   handle other launch command codes */
DWord PilotMain(Word cmd, Ptr cmdPBP, Word launchFlags)
{
  if (cmd == sysAppLaunchCmdNormalLaunch) {
    Word err;


    err = StartApplication();
    if (err) {
      FrmCloseAllForms();
      return err;
    }
	
    EventLoop();
    StopApplication();
	
  } else {
    return sysErrParamErr;
  }

  return 0;
}
