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

#include <GLICT/globals.h>
#include <GLICT/messagebox.h>
#include "gm_mainmenu.h"
#include "gm_gameworld.h"
#include "defines.h"
#include "engine.h"
#include "sprite.h"
#include "options.h"

#include "net/connection.h"
#include "net/protocollogin.h"
#include "net/protocolgame80.h"

extern Connection* g_connection;
extern bool g_running;


GM_MainMenu::GM_MainMenu()
{
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Starting main menu...\n");

	/* ****************** MAIN MENU *********************** */
	desktop.AddObject(&pnlMainMenu.mainmenu);
	pnlMainMenu.btnLogIn.SetOnClick(GM_MainMenu::pnlMainMenu_btnLogIn_OnClick);
	pnlMainMenu.btnOptions.SetOnClick(GM_MainMenu::pnlMainMenu_btnOptions_OnClick);
	pnlMainMenu.btnAbout.SetOnClick(GM_MainMenu::pnlMainMenu_btnAbout_OnClick);
	pnlMainMenu.btnExit.SetOnClick(GM_MainMenu::pnlMainMenu_btnExit_OnClick);


	/* ****************** LOGIN SCREEN *********************** */
	desktop.AddObject(&winLogin.window);
	winLogin.btnOk.SetOnClick(GM_MainMenu::winLogin_btnOk_OnClick);
	winLogin.btnCancel.SetOnClick(GM_MainMenu::winLogin_btnCancel_OnClick);

	/* ****************** CHARLIST SCREEN *********************** */
	desktop.AddObject(&winCharlist.window);
	winCharlist.btnOk.SetOnClick(GM_MainMenu::winCharlist_btnOk_OnClick);
	winCharlist.btnCancel.SetOnClick(GM_MainMenu::winCharlist_btnCancel_OnClick);

	/* ******************* OPTIONS ************************* */
	desktop.AddObject(&winOptions.window);
	winOptions.btnGeneral.SetOnClick(GM_MainMenu::winOptions_btnGeneral_OnClick);
	winOptions.btnGraphics.SetOnClick(GM_MainMenu::winOptions_btnGraphics_OnClick);
	winOptions.btnNetwork.SetOnClick(GM_MainMenu::winOptions_btnNetwork_OnClick);
	winOptions.btnMotd.SetOnClick(GM_MainMenu::winOptions_btnMotd_OnClick);
	winOptions.btnOk.SetOnClick(GM_MainMenu::winOptions_btnOk_OnClick);

	/* ***************** OPTIONS/GENERAL ******************** */
	desktop.AddObject(&winOptionsGeneral.window);
	winOptionsGeneral.btnOk.SetOnClick(GM_MainMenu::winOptionsGeneral_btnOk_OnClick);
	winOptionsGeneral.btnCancel.SetOnClick(GM_MainMenu::winOptionsGeneral_btnCancel_OnClick);
	winOptionsGeneral.btnHelp.SetOnClick(GM_MainMenu::btnHelp_OnClick);
	winOptionsGeneral.btnHelp.SetCustomData(new std::string(
		"Classic Control is ...\n"
		"\n"
		"Auto-chase Off turns off automatic chase of any\n"
		"monster you attack.\n"
		"\n"
		"Show hints allows occasional popups with some help\n"
		"during gameplay.\n"
		"\n"
		"Show names determines if there will be floating names\n"
		"above creatures.\n"
		"\n"
		"Show text effects allows you to turn on and off the\n"
		"so-called \"animated text\" effects."
		));

	/* ***************** OPTIONS/GRAPHICS ******************** */
	desktop.AddObject(&winOptionsGraphics.window);
	winOptionsGraphics.btnOk.SetOnClick(GM_MainMenu::winOptionsGraphics_btnOk_OnClick);
	winOptionsGraphics.btnCancel.SetOnClick(GM_MainMenu::winOptionsGraphics_btnCancel_OnClick);
	winOptionsGraphics.btnHelp.SetOnClick(GM_MainMenu::btnHelp_OnClick);
	winOptionsGraphics.btnHelp.SetCustomData(new std::string(

		"Fullscreen decides whether or not the client will\n"
		"be taking the entire screen.\n"
		"\n"
		"Available resolutions shows detected resolutions on\n"
		"your computer that the client can use while in fullscreen."
		));

	/* ***************** OPTIONS/NETWORK ******************** */
	desktop.AddObject(&winOptionsNetwork.window);
	winOptionsNetwork.btnOk.SetOnClick(GM_MainMenu::winOptionsNetwork_btnOk_OnClick);
	winOptionsNetwork.btnCancel.SetOnClick(GM_MainMenu::winOptionsNetwork_btnCancel_OnClick);
	winOptionsNetwork.btnHelp.SetOnClick(GM_MainMenu::btnHelp_OnClick);
	winOptionsNetwork.btnHelp.SetCustomData(new std::string(
		"Server name is the IP address or hostname of the\n"
		"login server.\n"
		"\n"
		"Port is the TCP port of the login server.\n"
		"\n"
		"You can also choose if you'd like to access OpenTibia\n"
		"servers, or a proprietary server. To access OpenTibia\n"
		"servers, turn on the OT Encryption Key option."
		));




	pnlMainMenu.btnLogIn.Focus(NULL);

	if (options.ui_compat) {
		#if (GLICT_APIREV >= 50)
		glictGlobals.drawFocus = true;
		#else
		#warning Drawing focus requires at least GLICT revision 50. Please upgrade GLICT. (Compiling will proceed, but this feature will not be available.)
		#endif
	} else {
		#if (GLICT_APIREV >= 50)
		glictGlobals.drawFocus = false;
		#endif
	}

	if (g_engine) {
		background = g_engine->createSprite("Tibia.pic",0);
		if(!background->isLoaded()){
			delete background;
			background = NULL;
		}
	}
	else{
		background = NULL;
	}

	doResize(glictGlobals.w, glictGlobals.h);

	renderScene();
}

GM_MainMenu::~GM_MainMenu()
{
	delete background;
}


void GM_MainMenu::updateScene()
{
    if (options.engine == ENGINE_OPENGL) {
        renderScene();
    }
}

void GM_MainMenu::doResize(float w, float h)
{
	pnlMainMenu.mainmenu.SetPos(60, glictGlobals.h - 240);
	desktop.SetWidth(glictGlobals.w);
	desktop.SetHeight(glictGlobals.h);
	desktop.ResetTransformations();
	centerWindow(&winLogin.window);
	centerWindow(&winCharlist.window);
	centerWindow(&winOptions.window);
	centerWindow(&winOptionsGeneral.window);
	centerWindow(&winOptionsGraphics.window);
	centerWindow(&winOptionsNetwork.window);
	centerWindow(&winStatus);

	renderScene();
}

void GM_MainMenu::renderScene()
{
	if(background)
		background->Blit(0,0,0,0,background->getWidth(),background->getHeight(),glictGlobals.w, glictGlobals.h);

	renderUI();
}

void GM_MainMenu::renderUI()
{
#ifdef USE_OPENGL
	if (options.engine == ENGINE_OPENGL) {
		glEnable(GL_SCISSOR_TEST);
		desktop.Paint();
		glDisable(GL_SCISSOR_TEST);
	} else {
		desktop.Paint();
	}
#else
	desktop.Paint();
#endif
}
void GM_MainMenu::mouseEvent(SDL_Event& event)
{
	glictPos pos;
	// FIXME (ivucica#3#) this is incorrect, we should be refreshing ptrx and ptry here as well, not just read the old versions ...
	// who knows how the platforms with a different pointing device (e.g. touchscreen?) would behave!
	pos.x = ptrx;
	pos.y = ptry;

	desktop.TransformScreenCoords(&pos);

	if (event.button.state == SDL_PRESSED)
		desktop.CastEvent(GLICT_MOUSEDOWN, &pos, 0);
	if (event.button.state != SDL_PRESSED)
		desktop.CastEvent(GLICT_MOUSEUP, &pos, 0);

	renderScene();

}

void GM_MainMenu::keyPress (char key)
{
	//printf("Key %c %d\n", key, key);
	if (!options.ui_compat) {
		if (glictGlobals.topFocused == &winLogin.txtUsername && key == 13)
			winLogin.btnOk.Focus(NULL);
		else if (glictGlobals.topFocused == &winLogin.txtPassword && key == 13)
			winLogin.btnOk.Focus(NULL);
	} else {
		if (glictGlobals.topFocused == &winLogin.txtUsername && key == 13) {
			winLogin.txtPassword.Focus(NULL);
			renderUI();
			return;
		}
		else if (glictGlobals.topFocused == &winLogin.txtPassword && key == 13)
			winLogin.btnOk.Focus(NULL);

	}

	desktop.CastEvent(GLICT_KEYPRESS, &key, 0);
	renderUI();
}

void GM_MainMenu::centerWindow (glictWindow *win)
{
	glictSize s;
	win->GetSize(&s);
	win->SetPos(glictGlobals.w / 2 - s.w / 2, glictGlobals.h/2 - s.h / 2);
}

void GM_MainMenu::msgBox (const char* mbox, const char* title, glictContainer* focusondismiss)
{
	glictSize s;
	glictMessageBox *mb;
	desktop.AddObject(mb = new glictMessageBox);

	mb->SetCaption(title);
	mb->SetMessage(mbox);

	mb->SetHeight(glictFontNumberOfLines(mbox)*14 + 35);
	int size1 = (int)glictFontSize(title, "system");
	int size2 = (int)glictFontSize(mbox, "system");
	mb->SetWidth(MAX(size1, size2));
	mb->Focus(NULL);

	mb->GetSize(&s);

	mb->SetPos(glictGlobals.w / 2 - s.w / 2, glictGlobals.h/ 2 - s.h / 2);

	mb->SetOnDismiss(GM_MainMenu::MBOnDismiss);

	mb->SetCustomData(focusondismiss);

}

void GM_MainMenu::MBOnDismiss(glictPos* pos, glictContainer* caller)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	if (caller->GetCustomData())
		((glictContainer*)caller->GetCustomData())->Focus(NULL);
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Repaint.\n");
	m->renderScene();
	//delete caller;
}


/* ****** EVENT CALLBACKS ****** */
void GM_MainMenu::pnlMainMenu_btnLogIn_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winLogin.window.SetVisible(true);
	m->winLogin.txtUsername.SetCaption(options.account);
	m->winLogin.txtPassword.SetCaption(options.password);
	m->winLogin.txtUsername.Focus(NULL);
}

void GM_MainMenu::pnlMainMenu_btnOptions_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptions.window.SetVisible(true);
	m->winOptions.window.Focus(NULL);
}

void GM_MainMenu::pnlMainMenu_btnAbout_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	std::stringstream txt;
	unsigned char c = 169;
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	txt << "YATC - Yet Another Tibia Client\n"
		<< "v0.2 SVN\n"
		<< "\n"
		<< c <<" 2007-2008 OpenTibia Team\n"
		<< "\n"
		<< "Programmed by (in no particular order):\n"
		<< "mips\n"
		<< "Ivan Vucica\n"
		<< "Smygflik\n"
		<< "\n"
		<< "YATC comes with ABSOLUTELY NO WARRANTY; \n"
		<< "for details see sections 11 and 12 in COPYING.\n"
		<< "This is free software, and you are welcome \n"
		<< "to redistribute it under certain conditions;\n"
		<< "see COPYING for details.";

	m->msgBox(txt.str().c_str(), "About YATC", &m->pnlMainMenu.btnAbout);
}

void GM_MainMenu::pnlMainMenu_btnExit_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	g_running = 0;
}
void GM_MainMenu::btnHelp_OnClick(glictPos* relmousepos, glictContainer* callerclass) {
	std::string *helptext = (std::string*)callerclass->GetCustomData();
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->msgBox(helptext->c_str(), "Help", callerclass);
}

/* **********LOGIN******* */
void GM_MainMenu::winLogin_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winLogin.window.SetVisible(false);

	if(g_connection){
		delete g_connection;
		g_connection = NULL;
	}

	ClientVersion_t proto = options.protocol;
	if (!proto)
		proto = ProtocolConfig::detectVersion();

	if (!proto) {
		std::stringstream t;
		unsigned char c = 149; // bullet
		t << "Data files in the directory either:\n" <<
			 c << "do not belong to same protocol version, or\n" <<
			 c << "are modified and with unknown signatures.\n" <<
			 "\n" <<
			 "Either get correct files from a single known\n" <<
			 "client version, or make sure that you have\n" <<
			 "correct signatures in the files, or go to\n" <<
			 "Options->Network and choose the correct\n" <<
			 "protocol.";


		m->msgBox(t.str().c_str(),"Protocol detection failed");
		return;
	} /*else {
		std::stringstream t;
		t << "Autodetected " << std::dec << proto << " dat: 0x" << std::hex << ProtocolConfig::getInstance().getDatSignature() << " pic: 0x" << std::hex << ProtocolConfig::getInstance().getPicSignature() << " spr: 0x" << std::hex << ProtocolConfig::getInstance().getSprSignature();
		m->msgBox(t.str().c_str(),"Protocol detection");
	}*/

	ProtocolConfig::getInstance().setServerType(options.otkey ? SERVER_OTSERV : SERVER_CIP ); // perhaps this should go to options, too?
	ProtocolConfig::getInstance().setVersion(CLIENT_OS_WIN, proto); // TODO (ivucica#3#) see if we can freely tell the 'real' OS, meaning "linux" on unices, and "windows" on windows
	ProtocolConfig::getInstance().setVersionOverride(options.overrideversion);
	ProtocolConfig::getInstance().setServer(options.server, options.port);

	ProtocolConfig::createLoginConnection(atoi(m->winLogin.txtUsername.GetCaption().c_str()), m->winLogin.txtPassword.GetCaption());
	m->desktop.AddObject(&m->winStatus);
	m->centerWindow(&m->winStatus);
	m->winStatus.SetCaption("Logging in");
	m->winStatus.SetMessage("Connecting to the server...\n");
	m->winStatus.SetWidth(320);
	m->winStatus.SetEnabled(false);
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "SetVisible...\n");
}

void GM_MainMenu::winLogin_btnCancel_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winLogin.window.SetVisible(false);
}

/* **********CHARLIST******* */
void GM_MainMenu::winCharlist_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	// FIXME (ivucica#1#) mips should check if this switching to gameworld is written properly

	GM_MainMenu* m = (GM_MainMenu*)g_game;
	uint32_t ipnum = m->winCharlist.currentChar.ip;
	std::stringstream ips;
	std::string ip;

	ips << (ipnum & 0xFF) << "." << ((ipnum & 0xFF00) >> 8) << "." << ((ipnum & 0xFF0000) >> 16) << "." << ((ipnum & 0xFF000000) >> 24);
	ip = ips.str();

	m->winCharlist.window.SetVisible(false);
	if(g_connection){
		delete g_connection;
		g_connection = NULL;
	}

	ClientVersion_t proto = options.protocol;
	if (!proto) proto = ProtocolConfig::detectVersion();
	ASSERT(proto);

	ProtocolConfig::getInstance().setVersion(CLIENT_OS_WIN, proto); // TODO (ivucica#3#) see if we can freely tell the 'real' OS, meaning "linux" on unices, and "windows" on windows
	ProtocolConfig::getInstance().setServer(ip, m->winCharlist.currentChar.port);
	ProtocolConfig::getInstance().setVersionOverride(options.overrideversion);
	ProtocolConfig::createGameConnection(atoi(m->winLogin.txtUsername.GetCaption().c_str()), m->winLogin.txtPassword.GetCaption(), m->winCharlist.currentChar.name, false /* isgm*/);

	m->desktop.AddObject(&m->winStatus);
	m->centerWindow(&m->winStatus);
	m->winStatus.SetCaption("Entering game");
	m->winStatus.SetMessage("Connecting to the server...");

}

void GM_MainMenu::winCharlist_btnCancel_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winCharlist.window.SetVisible(false);
}

/* **********OPTIONS********* */

void GM_MainMenu::winOptions_btnGeneral_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsGeneral.Init();
	m->winOptionsGeneral.window.SetVisible(true);
	m->winOptionsGeneral.window.Focus(NULL);
}
void GM_MainMenu::winOptions_btnGraphics_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsGraphics.Init();
	m->winOptionsGraphics.window.SetVisible(true);
	m->winOptionsGraphics.window.Focus(NULL);
}
void GM_MainMenu::winOptions_btnNetwork_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsNetwork.Init();
	m->winOptionsNetwork.window.SetVisible(true);
	m->winOptionsNetwork.window.Focus(NULL);
}
void GM_MainMenu::winOptions_btnMotd_OnClick(glictPos* relmousepos, glictContainer* callerclass) {
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->msgBox(options.motdtext.c_str(), "Message of the Day", &m->winCharlist.lstChars );
}
void GM_MainMenu::winOptions_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass) {
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptions.window.SetVisible(false);
}
/* *********GENERAL********** */

void GM_MainMenu::winOptionsGeneral_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass) {
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsGeneral.Store();
	m->winOptionsGeneral.window.SetVisible(false);
}
void GM_MainMenu::winOptionsGeneral_btnCancel_OnClick(glictPos* relmousepos, glictContainer* callerclass) {
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsGeneral.window.SetVisible(false);
}
/* *********GENERAL********** */

void GM_MainMenu::winOptionsGraphics_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	//enginelist_t e = options.engine;
	//int w = options.w; int h = options.h; int bpp = options.bpp;  // these will be used in disabled chunk of code, do not remove
	//bool fs = options.fullscreen;

	GM_MainMenu* m = (GM_MainMenu*)g_game;

	m->winOptionsGraphics.Store();
	#if 0
	if (e != options.engine || w != options.w || h != options.h || bpp != options.bpp || fs != options.fullscreen) {
		switch(options.engine) {
			case ENGINE_OPENGL:
				g_engine = new EngineGL;
				break;
			/*
			case ENGINE_DIRECTX:
				g_engine = new EngineDX;
				break;
			*/
			default:
				DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "Unknown engine was selected. Falling back to SDL.");
				options.engine = ENGINE_SDL;
			case ENGINE_SDL:
				g_engine = new EngineSDL;
				break;
		}

		if(!g_engine->isSupported()){
			DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_WARNING, "The selected graphics engine is not supported. Falling back to SDL.");
			delete g_engine;
			options.engine = ENGINE_SDL;
			g_engine = new EngineSDL;
		}
	}
	#endif
	m->winOptionsGraphics.window.SetVisible(false);
}
void GM_MainMenu::winOptionsGraphics_btnCancel_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsGraphics.window.SetVisible(false);
}
/* **********NETWORK********* */
void GM_MainMenu::winOptionsNetwork_btnOk_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsNetwork.Store();
	m->winOptionsNetwork.window.SetVisible(false);
}

void GM_MainMenu::winOptionsNetwork_btnCancel_OnClick(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winOptionsNetwork.window.SetVisible(false);
}

/* ************* OTHER **************** */
void GM_MainMenu::winMotd_OnDismiss(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->winCharlist.window.SetVisible(true);
	m->renderScene();
}
void GM_MainMenu::winStatus_ErrorOnDismiss(glictPos* relmousepos, glictContainer* callerclass)
{
	GM_MainMenu* m = (GM_MainMenu*)g_game;
	m->pnlMainMenu.btnLogIn.Focus(NULL);
	m->renderScene();

}

/* ********** Responses to notifications *********** */
void GM_MainMenu::onConnectionError(int message, const char* errortext)
{
	std::stringstream s;
	s << //"There was an error while connecting." << std::endl <<
		 //std::endl <<
	     //"The following may help you identify error:" << std::endl <<
	     errortext;

	winStatus.SetCaption("Error");
	winStatus.SetMessage(s.str());
	winStatus.SetEnabled(true);
	renderScene();
}
void GM_MainMenu::openMOTD(int motdnum, const std::string& text)
{

	if (options.motdnum == motdnum) {
		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Motd numbers matching, not displaying motd.\n");
		desktop.RemoveObject(&winStatus);
		options.motdtext = text;
		winMotd_OnDismiss(NULL, NULL);
		renderScene();
		return;
	}

	options.motdnum = motdnum;
	options.motdtext = text;
	options.Save();

	winStatus.SetCaption("Message of the Day");
	winStatus.SetMessage(text);
	winStatus.SetEnabled(true);
	winStatus.Focus(NULL);

	winStatus.SetOnDismiss(winMotd_OnDismiss);
	renderScene();

}
void GM_MainMenu::openMessageWindow(WindowMessage_t type, const std::string& text)
{
	if (type == MESSAGE_ERROR)
		winStatus.SetCaption("Error");
	else
		winStatus.SetCaption("Information");

	winStatus.SetMessage(text);
	winStatus.SetEnabled(true);
	winStatus.Focus(NULL);
	winStatus.SetOnDismiss(winStatus_ErrorOnDismiss);
	renderScene();
}

void GM_MainMenu::openCharactersList(const std::list<CharacterList_t>& list, int premDays)
{
	winCharlist.generateList(list, premDays);
	renderUI();
}

void GM_MainMenu::onEnterGame()
{
	delete g_game;
	g_game = new GM_Gameworld;
}
