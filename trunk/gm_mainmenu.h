//////////////////////////////////////////////////////////////////////
// Yet Another Tibia Client
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __GM_MAINMENU_H
#define __GM_MAINMENU_H

#include <GLICT/container.h>
#include <GLICT/button.h>
#include <GLICT/window.h>
#include "gamemode.h"
#include "sprite.h"
class GM_MainMenu : public GameMode
{
public:
	GM_MainMenu();
	~GM_MainMenu();

	void renderScene();
private:
	glictContainer desktop;
	glictButton btnExit;
	struct {
		glictWindow login;
	} winLogin;

	Sprite* background;
};

#endif