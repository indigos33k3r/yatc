//////////////////////////////////////////////////////////////////////
// Yet Another Tibia Client
//////////////////////////////////////////////////////////////////////
// Main
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

/// \file main.cpp
/// This file contains the main(int,char**) function.

#define MAXFPS 10


#include <SDL/SDL.h>
#include <GLICT/globals.h>
#include <sstream>
#include <stdlib.h>
#include "debugprint.h"
#include "options.h"
#include "engine.h"
#include "objects.h"
#include "gamemode.h"
#include "gm_mainmenu.h"
#include "gm_debug.h"
#include "util.h"
#include "skin.h"
#include "config.h"
#include "spritesdl.h" // to load icon


#include "net/connection.h"
#include "net/protocollogin.h"
#include "net/protocolgame.h"
#include "gamecontent/map.h"
bool g_running = false;
//uint32_t keymods = 0;

Connection* g_connection = NULL;
uint32_t g_frameTime = 0;

int g_frames;

void onKeyDown(const SDL_Event& event)
{
	int key = event.key.keysym.sym;
	switch(key){
	case SDLK_ESCAPE:
		g_running = false;
		break;

	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		// ignore shiftpresses
		break;

	// TODO (ivucica#1#) Add pageup, pagedown, home, end below
	case SDLK_LEFT:
	case SDLK_RIGHT:
	case SDLK_UP:
	case SDLK_DOWN:
		g_game->specKeyPress(key);
		break;
	default:
		// glict expects what glut usually serves: completely prepared keys,
		// with shift already parsed and all that
		// we'll try to do the parsing here or elsewhere


		if(key < 32){
			switch(key){
			case SDLK_BACKSPACE:
			case SDLK_TAB:
			case SDLK_RETURN:
			case SDLK_ESCAPE:
				break;
			default:
				return;
			}
		}

		if((event.key.keysym.mod & KMOD_NUM) && key >= 256 && key <= 271){ //Numeric keypad
			switch(key){
			case SDLK_KP_PERIOD:
				key = SDLK_PERIOD;
				break;
			case SDLK_KP_DIVIDE:
				key = SDLK_SLASH;
				break;
	 		case SDLK_KP_MULTIPLY:
	 			key = SDLK_ASTERISK;
		 		break;
			case SDLK_KP_MINUS:
				key = SDLK_MINUS;
				break;
			case SDLK_KP_PLUS:
				key = SDLK_PLUS;
				break;
			case SDLK_KP_ENTER:
				key = SDLK_RETURN;
				break;
			default:
				key = event.key.keysym.unicode;
				break;
			}
		} else {
		    key = event.key.keysym.unicode;
		}

		if(key > 255)
			return;

		g_game->keyPress(key);
	}
}

void checkFile(const char *filename)
{
	if (!fileexists(filename)) {
		std::stringstream s;
		s << "Loading the data file '" << filename << "' has failed.\nPlease place '" << filename << "' in the same folder as YATC.";
		NativeGUIError(s.str().c_str(), "YATC Fatal Error");
		exit(1);
	}

}

void checkFiles()
{
	checkFile("Tibia.pic");
	checkFile("Tibia.spr");
	checkFile("Tibia.dat");
}

void setIcon()
{
	g_engine = NULL;
	SDL_WM_SetCaption("YATC v0.2 SVN", "YATC v0.2 SVN");
	SpriteSDL *s = new SpriteSDL("Tibia.spr", 13855);
	SpriteSDL *st = new SpriteSDL("Tibia.spr", 13575);
	s->templatedColorize(st, 79, 94, 88, 82);
	s->setAsIcon();
	delete s;
	delete st;
}

/// \brief Main program function
///
/// This function does very little on its own. It manages some output to
/// player's console, directs subsystems to initialize themselves and makes
/// choice of rendering engine. Then it runs the main game loop, processing
/// events and sending them further into the game.
int main(int argc, char *argv[])
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );

	if(WSAStartup(wVersionRequested, &wsaData) != 0){
		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Winsock startup failed!!");
		return -1;
	}

	if((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2)){
		WSACleanup( );
		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "No Winsock 2.2 found!");
		return -1;
	}

#endif


	//setenv("SDL_VIDEODRIVER", "aalib", 0);
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "YATC -- YET ANOTHER TIBIA CLIENT\n");
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "================================\n");
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "version 0.2 SVN\n");
#ifdef BUILDVERSION
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "build %s\n", BUILDVERSION);
#endif
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, " This is free software: you are free to change and redistribute it.\n"
		" There is NO WARRANTY, to the extent permitted by law. \n"
		" Review LICENSE in YATC distribution for details.\n");


	yatc_fopen_init();


	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Checking graphics files existence...");
	checkFiles();
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, " passed\n");

	options.Load();


	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Initializing windowing...\n");

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0){
		std::stringstream out;
		out << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
		DEBUGPRINT(DEBUGPRINT_ERROR, DEBUGPRINT_LEVEL_OBLIGATORY, out.str().c_str());

		NativeGUIError(out.str().c_str(), "YATC Fatal Error");
		exit(1);
	}

	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Loading data...\n");
	if(!Objects::getInstance()->loadDat("Tibia.dat")){
		DEBUGPRINT(DEBUGPRINT_ERROR, DEBUGPRINT_LEVEL_OBLIGATORY, "Loading data file failed!");
		NativeGUIError("Loading the data file 'Tibia.dat' has failed.\nPlease place 'Tibia.dat' in the same folder as YATC.", "YATC Fatal Error");
		exit(1);
	}


	setIcon(); // must be called prior to first call to SDL_SetVideoMode() (currently done in engine)

	SDL_EnableKeyRepeat(200, 50);
	SDL_EnableUNICODE(1);

	try{
		switch(options.engine){
			#ifdef USE_OPENGL
			case ENGINE_OPENGL:
				g_engine = new EngineGL;
				break;
			#endif

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


		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Loading skin...\n");
		g_skin.loadSkin();
		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Skin has been loaded\n");


		DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Constructing gamemode...\n");
		g_game = new GM_MainMenu();
		//g_game = new GM_Debug(); // ivucica: this is for testing -- choice should be a cmd line option

		DEBUGPRINT(DEBUGPRINT_LEVEL_OBLIGATORY, DEBUGPRINT_NORMAL, "Running\n");
        g_running = true;

		SDL_Event event;
		while(g_running){

            g_engine->fpsMutexLock();

            int beginticks = SDL_GetTicks();
			//first process sdl events
			while(SDL_PollEvent(&event)){
				switch (event.type){
					case SDL_VIDEORESIZE:
						g_engine->doResize(event.resize.w, event.resize.h);
						g_game->doResize(event.resize.w, event.resize.h);
						break;

					case SDL_QUIT:
						g_running = false;
						break;

					case SDL_KEYUP:
						/*if(event.key.keysym.sym == SDLK_LSHIFT ||
							event.key.keysym.sym == SDLK_RSHIFT){
							keymods = keymods & ~(uint32_t)KMOD_SHIFT;
						}*/
						break;

					case SDL_KEYDOWN:
						onKeyDown(event);
						break;

					case SDL_MOUSEBUTTONUP:
					case SDL_MOUSEBUTTONDOWN:
						#ifdef WINCE
						if (ptrx < 5 && ptry < 5)
							SDL_WM_IconifyWindow(); // appears to crash the application?! ah nevermind
						#endif
						g_game->mouseEvent(event);
						break;

					case SDL_MOUSEMOTION:
						ptrx = event.motion.x;
						ptry = event.motion.y;
						break;
					default:
						break;
				}
			}
			//update current frame time
			g_frameTime = SDL_GetTicks();


            if(g_frameTime - beginticks < 1000/MAXFPS - 10){
                SDL_Delay(1000/MAXFPS - (g_frameTime - beginticks) - 10);
            }

            while (abs((int)SDL_GetTicks() - beginticks) < 1000/MAXFPS);

			//check connection
			if(g_connection){
				g_connection->executeNetwork();
			}

            if (!(SDL_GetAppState() & SDL_APPACTIVE)) {// if the application is minimized
                #ifdef WIN32
                Sleep(100); // sleep a while, and don't paint
                #else
                usleep(100 * 1000);
                #endif
            } else { //otherwise update scene
                g_game->updateScene();
                g_engine->Flip();
            }
			g_frames ++;

			g_engine->fpsMutexUnlock();

		}
	}
	catch(std::string errtext){
		DEBUGPRINT(DEBUGPRINT_ERROR, DEBUGPRINT_LEVEL_OBLIGATORY, "%s", errtext.c_str());
	}

	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Game over\n");

	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Unloading data...\n");
	Objects::getInstance()->unloadDat();
	delete Objects::getInstance();
	g_skin.unloadSkin();
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Terminating protocol connection and unloading related data...\n");
	delete g_connection;
	Map::getInstance().clear();
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Saving options...\n");
	options.Save();
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Finishing engine...\n");
	delete g_engine;
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Ending game...\n");
	delete g_game;
	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Shutting down SDL...\n");
	SDL_Quit();

#ifdef WIN32
	WSACleanup();
#endif

	DEBUGPRINT(DEBUGPRINT_NORMAL, DEBUGPRINT_LEVEL_OBLIGATORY, "Thanks for playing!\n");

	return 0;
}
