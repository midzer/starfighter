/*
Project: Starfighter
Copyright (C) 2003 Parallel Realities
Copyright (C) 2011, 2012 Guus Sliepen
Copyright (C) 2015-2023 Diligent Circle <diligentcircle@riseup.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#include "SDL.h"

#ifndef NOSOUND
#include "SDL_mixer.h"
#endif

#include "colors.h"
#include "defs.h"
#include "structs.h"

#include "alien.h"
#include "audio.h"
#include "cutscene.h"
#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "intermission.h"
#include "renderer.h"
#include "screen.h"
#include "title.h"
#include "weapons.h"

int main(int argc, char **argv)
{
	int cheatAttempt;
	int cheatCount;
	int section;

#ifdef __APPLE__
	// This makes relative paths work in Xcode by changing directory to the Resources folder inside the .app bundle
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
	{
		chdir(path);
		printf("Current directory \"%s\"\n", path);
	}
	else
	{
		if (chdir(DATADIR) == -1)
			printf("Warning: failed to change directory to \"%s\"\n", DATADIR);
	}
#else
	if (chdir(DATADIR) == -1)
		printf("Warning: failed to change directory to \"%s\"\n", DATADIR);
#endif

	engine_init(); // Must do this first!

	cheatAttempt = 0;
	cheatCount = 0;

	if (argc > 1)
	{
		if (strcmp("--help", argv[1]) == 0)
		{
			printf("\nProject: Starfighter %s\n", VERSION);
			printf("Copyright Parallel Realities 2003\n");
			printf("Copyright Guus Sliepen, Astrid S. de Wijn and others 2012\n");
			printf("Additional Commands\n");
			printf("\t-noaudio     Disables sound and music\n");
			printf("\t-mono        Mono sound output (best for headphones)\n");
			printf("\t-oldscript   Uses original script from version 1.1\n");
			printf("\n");
			printf("https://pr-starfighter.github.io\n");
			printf("\n");
			exit(0);
		}
	}

	for (int i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-cheat") == 0)
			cheatAttempt = 1;
		if (strcmp(argv[i], "-noaudio") == 0)
		{
			printf("No Audio\n");
			engine.useAudio = 0;
		}
		if (strcmp(argv[i], "-oldscript") == 0)
		{
			printf("Using script from Project: Starfighter 1.1\n");
			engine.oldScript = 1;
		}
		if ((strcmp(argv[i], "humans") == 0) && (cheatCount == 0))
			cheatCount = 1;
		if ((strcmp(argv[i], "do") == 0) && (cheatCount == 1))
			cheatCount = 2;
		if ((strcmp(argv[i], "it") == 0) && (cheatCount == 2))
			cheatCount = 3;
		if (((strcmp(argv[i], "better") == 0) && (cheatCount == 3))
				|| (strcmp(argv[i], "humansdoitbetter") == 0))
		{
			printf("Humans do it better! Cheats enabled.\n");
			engine.cheat = 1;
		}
	}

	atexit(engine_cleanup);

	gfx_init();
	engine_setMode(); // Settings get loaded here
	gfx_loadFont();

	if ((strcmp(engine.lang, "default") == 0)
			|| (strcmp(engine.lang, "") == 0))
		setlocale(LC_ALL, "");
	else
		setlocale(LC_ALL, engine.lang);

	bindtextdomain("pr-starfighter", "./locale/");
	textdomain("pr-starfighter");

	if (cheatAttempt && !engine.cheat)
	{
		screen_clear(black);
		screen_renderString("That doesn't work anymore", -1, 285, FONT_WHITE);
		screen_renderString("Try harder...", -1, 315, FONT_WHITE);
		renderer_update();
		SDL_Delay(2000);
		renderer_update();
		SDL_Delay(500);
	}

	gfx_free();
	audio_loadSounds();

	weapons_init();

	srand(time(NULL));

#ifndef NOSOUND
	if (engine.useAudio)
	{
		Mix_Volume(-1, 100);
		Mix_VolumeMusic(engine.musicVolume);
	}
#endif

	alien_defs_init();

	colors_init();

	// Determine which part of the game we will go to...
	section = 0;

	game.difficulty = DIFFICULTY_EASY;
	game_init();

	while (1)
	{
		switch (section)
		{
			case 0:
				section = title_show();
				break;

			case 1:
				section = intermission();
				break;

			case 2:
				if (game.stationedPlanet == -1)
					cutscene_init(0);
				section = game_mainLoop();
				break;
		}
	}

	return(0);
}
