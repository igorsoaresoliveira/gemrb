/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header: /data/gemrb/cvs2svn/gemrb/gemrb/gemrb/plugins/Core/Control.cpp,v 1.39 2005/11/12 19:31:51 avenger_teambg Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include "../../includes/win32def.h"
#include "Control.h"
#include "ControlAnimation.h"
#include "Window.h"
#include "Interface.h"

Control::Control()
{
	hasFocus = false;
	Changed = true;
	InHandler = false;
	VarName[0] = 0;
	Value = 0;
	Flags = 0;
	Tooltip = NULL;
	Owner = 0;
	XPos = 0;
	YPos = 0;

	animation = NULL;
	AnimPicture = NULL;
}

Control::~Control()
{
	if (InHandler) {
		printf("[Control] Destroying control inside event handler, crash may occur!");
	}
	core->DisplayTooltip( 0, 0, NULL );
	if (Tooltip) {
		free (Tooltip);
	}

	if (animation) {
		delete animation;
	}
}

/** Sets the Tooltip text of the current control */
int Control::SetTooltip(const char* string, int /*pos*/)
{
	if (Tooltip)
		free(Tooltip);

	if ((string == NULL) || (string[0] == 0)) {
		Tooltip = NULL;
	} else {
		Tooltip = strdup (string);
	}
	Changed = true;
	return 0;
}

/** Sets the tooltip to be displayed on the screen now */
void Control::DisplayTooltip()
{
	if (Tooltip)
		core->DisplayTooltip( (( Window* )Owner)->XPos + XPos + Width / 2, (( Window* )Owner)->YPos + YPos + Height / 2, this );
	else
		core->DisplayTooltip( 0, 0, NULL );
}

void Control::ResetEventHandler(EventHandler handler)
{
	handler[0] = 0;
}

void Control::SetEventHandler(EventHandler handler, char* funcName)
{
	strncpy( handler, funcName, sizeof( EventHandler ) );
}

bool Control::SetEvent(int /*eventType*/, EventHandler /*handler*/)
{
	return false;
}

void Control::RunEventHandler(EventHandler handler)
{
	if (InHandler) {
		printf("[Control] Nested event handlers are not supported!");
		return;
	}
	if (handler[0]) {
		InHandler = true;
		core->GetGUIScriptEngine()->RunFunction( (char*)handler );
		//we should make sure the event didn't destruct this
		//object otherwise we overwrite memory
		InHandler = false;
	}
}

/** Key Press Event */
void Control::OnKeyPress(unsigned char /*Key*/, unsigned short /*Mod*/)
{
	//printf("OnKeyPress: CtrlID = 0x%08X, Key = %c (0x%02hX)\n", (unsigned int) ControlID, Key, Key);
}

/** Key Release Event */
void Control::OnKeyRelease(unsigned char /*Key*/, unsigned short /*Mod*/)
{
	//printf( "OnKeyRelease: CtrlID = 0x%08X, Key = %c (0x%02hX)\n", (unsigned int) ControlID, Key, Key );
}

/** Mouse Enter Event */
void Control::OnMouseEnter(unsigned short /*x*/, unsigned short /*y*/)
{
//	printf("OnMouseEnter: CtrlID = 0x%08X, x = %hd, y = %hd\n", (unsigned int) ControlID, x, y);
}

/** Mouse Leave Event */
void Control::OnMouseLeave(unsigned short /*x*/, unsigned short /*y*/)
{
//	printf("OnMouseLeave: CtrlID = 0x%08X, x = %hd, y = %hd\n", (unsigned int) ControlID, x, y);
}

/** Mouse Over Event */
void Control::OnMouseOver(unsigned short /*x*/, unsigned short /*y*/)
{
	//printf("OnMouseOver: CtrlID = 0x%08X, x = %hd, y = %hd\n", (unsigned int) ControlID, x, y);
}

/** Mouse Button Down */
void Control::OnMouseDown(unsigned short /*x*/, unsigned short /*y*/,
	unsigned char /*Button*/, unsigned short /*Mod*/)
{
	//printf("OnMouseDown: CtrlID = 0x%08X, x = %hd, y = %hd, Button = %d, Mos = %hd\n", (unsigned int) ControlID, x, y, Button, Mod);
}

/** Mouse Button Up */
void Control::OnMouseUp(unsigned short /*x*/, unsigned short /*y*/,
	unsigned char /*Button*/, unsigned short /*Mod*/)
{
	//printf("OnMouseUp: CtrlID = 0x%08X, x = %hd, y = %hd, Button = %d, Mos = %hd\n", (unsigned int) ControlID, x, y, Button, Mod);
}

/** Special Key Press */
void Control::OnSpecialKeyPress(unsigned char /*Key*/)
{
	//printf("OnSpecialKeyPress: CtrlID = 0x%08X, Key = %d\n", (unsigned int) ControlID, Key);
}

/** Sets the Display Flags */
int Control::SetFlags(int arg_flags, int opcode)
{
	if ((arg_flags >>24) != ControlType)
		return -2;
	switch (opcode) {
		case BM_SET:
			Flags = arg_flags;  //set
			break;
		case BM_AND:
			Flags &= arg_flags;
			break;
		case BM_OR:
			Flags |= arg_flags; //turn on
			break;
		case BM_XOR:
			Flags ^= arg_flags;
			break;
		case BM_NAND:
			Flags &= ~arg_flags;//turn off
			break;
		default:
			return -1;
	}
	Changed = true;
	( ( Window * ) Owner )->Invalidate();
	return 0;
}

void Control::SetAnimPicture(Sprite2D* newpic)
{
	AnimPicture = newpic;
	Changed = true;
	//Flags |= IE_GUI_BUTTON_PICTURE;
	//( ( Window * ) Owner )->Invalidate();
}

