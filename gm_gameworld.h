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

#ifndef __GM_GAMEWORLD_H
#define __GM_GAMEWORLD_H

#include "gamemode.h"
#include "sprite.h"
#include "mapui.h"
#include "ui/inventory.h"
#include "ui/skills.h"
#include "ui/health.h"

class Console;



class Tile;

class GM_Gameworld : public GameMode {
public:
	GM_Gameworld();
	~GM_Gameworld();

	void updateScene();
	void keyPress (char key);
	void specKeyPress (const SDL_keysym& key);
	void doResize(float w, float h);
	void mouseEvent(SDL_Event& event);


	virtual void onConnectionClosed();

	void onWalk();
	void onCancelWalk();
	void onTextMessage(MessageType_t type, const std::string& message);
	void onCreatureSpeak(SpeakClasses_t type, int n, const std::string& name, int level, const Position& pos, const std::string& message);
	void onCreatureSpeak(SpeakClasses_t type, int n, const std::string& name, int level, int channel, const std::string& message);
	void onCreatureSpeak(SpeakClasses_t type, int n, const std::string& name, int level, const std::string& message);
	void onCreatureMove(uint32_t id);
	void onChangeSkills();
	void onChangeStats();
protected:

private:
    std::vector<Console*>::iterator getDefaultConsole_it() { return m_consoles.begin(); }
    std::vector<Console*>::iterator findConsole_it(uint32_t channelid);
    std::vector<Console*>::iterator findConsole_it(const std::string& speaker);
    std::vector<Console*>::iterator findConsole_it(const Console* c);



    Console* getDefaultConsole() { return *getDefaultConsole_it(); }
    Console* findConsole(uint32_t channelid) { return *findConsole_it(channelid); }
    Console* findConsole(const std::string& speaker) { return *findConsole_it(speaker); }

    static void pnlConsoleButton_OnClick(glictPos* relmousepos, glictContainer* caller);

    void createConsole(uint32_t channelid=0, const std::string& speaker="");
    void createConsole(const std::string& speaker) { createConsole(0,speaker); }

    Console* getActiveConsole() const {return m_activeconsole;}
    void setActiveConsole(Console* i) {m_activeconsole = i;}

	Sprite* ui;
	glictContainer desktop;
	pnlInventory_t pnlInventory;
	pnlHealth_t pnlHealth;
	winSkills_t winSkills;
	glictPanel pnlTraffic;
	glictTextbox txtConsoleEntry;
	std::vector<glictPanel*> pnlConsoleButtons;
    glictPanel pnlConsoleButtonContainer;


	std::vector<Console*> m_consoles;
	Console* m_activeconsole;

	uint32_t m_startTime;

	class ProtocolGame* m_protocol;
	friend class pnlInventory_t;

	MapUI m_mapui;
};

#endif

