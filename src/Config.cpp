// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "WindowsWrapper.h"

#include "Backends/Misc.h"
#include "Config.h"
#include "File.h"
#include "Main.h"

const char* const gConfigName = "ConfigCSE2E.dat";
const char* const gProof = "CSE2E   20200430";

BOOL LoadConfigData(CONFIGDATA *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIGDATA));

	// Get path
	std::string path = gModulePath + '/' + gConfigName;

	// Open file
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp == NULL)
		return FALSE;

	// Read the version id and font name
	fread(conf->proof, sizeof(conf->proof), 1, fp);
	fread(conf->font_name, sizeof(conf->font_name), 1, fp);

	// Read display mode
	conf->display_mode = fgetc(fp);

	// Read framerate toggle
	conf->b60fps = fgetc(fp);

	// Read vsync toggle
	conf->bVsync = fgetc(fp);

	// Read smooth-scrolling toggle
	conf->bSmoothScrolling = fgetc(fp);

	// Read soundtrack value
	conf->soundtrack = fgetc(fp);

	// Read autojump toggle
	conf->bAutojump = fgetc(fp);

	// Read alt font toggle
	conf->bAltText = fgetc(fp);

	// Read alt font toggle
	conf->bNoDamage = fgetc(fp);

	// Read key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		conf->bindings[i].keyboard = File_ReadLE32(fp);
		conf->bindings[i].controller = fgetc(fp);
	}

	// Close file
	fclose(fp);

	// Check if version is not correct, and return if it failed
	if (strcmp(conf->proof, gProof))
	{
		memset(conf, 0, sizeof(CONFIGDATA));
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfigData(const CONFIGDATA *conf)
{
	// Get path
	std::string path = gModulePath + '/' + gConfigName;

	// Open file
	FILE *fp = fopen(path.c_str(), "wb");
	if (fp == NULL)
		return FALSE;

	// Write the version id and font name
	fwrite(conf->proof, sizeof(conf->proof), 1, fp);
	fwrite(conf->font_name, sizeof(conf->font_name), 1, fp);

	// Write display mode
	fputc(conf->display_mode, fp);

	// Write framerate toggle
	fputc(conf->b60fps, fp);

	// Write vsync toggle
	fputc(conf->bVsync, fp);

	// Write smooth-scrolling toggle
	fputc(conf->bSmoothScrolling, fp);

	// Write soundtrack value
	fputc(conf->soundtrack, fp);

	// Write Autojump value
	fputc(conf->bAutojump, fp);

	// Write Autojump value
	fputc(conf->bAltText, fp);

	// Write Autojump value
	fputc(conf->bNoDamage, fp);

	// Write key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		File_WriteLE32(conf->bindings[i].keyboard, fp);
		fputc(conf->bindings[i].controller, fp);
	}

	// Close file
	fclose(fp);

	return TRUE;
}

void DefaultConfigData(CONFIGDATA *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIGDATA));

	strcpy(conf->proof, gProof);

	// Fun fact: The Linux port added this line:
	// conf->display_mode = 1;

#ifdef _3DS
	conf->display_mode = 1;
#elif defined(__riscos__)
	conf->display_mode = 2;
#endif

	// Reset joystick settings (as these can't simply be set to 0)
	conf->bindings[BINDING_UP].controller = 7;
	conf->bindings[BINDING_DOWN].controller = 8;
	conf->bindings[BINDING_LEFT].controller = 9;
	conf->bindings[BINDING_RIGHT].controller = 10;
	conf->bindings[BINDING_OK].controller = 1;
	conf->bindings[BINDING_CANCEL].controller = 0;
	conf->bindings[BINDING_JUMP].controller = 1;
	conf->bindings[BINDING_SHOT].controller = 0;
	conf->bindings[BINDING_ARMSREV].controller = 3;
	conf->bindings[BINDING_ARMS].controller = 4;
	conf->bindings[BINDING_ITEM].controller = 5;
	conf->bindings[BINDING_MAP].controller = 2;
	conf->bindings[BINDING_PAUSE].controller = 6;

	// Set default key bindings
	conf->bindings[BINDING_UP].keyboard = BACKEND_KEYBOARD_UP;
	conf->bindings[BINDING_DOWN].keyboard = BACKEND_KEYBOARD_DOWN;
	conf->bindings[BINDING_LEFT].keyboard = BACKEND_KEYBOARD_LEFT;
	conf->bindings[BINDING_RIGHT].keyboard = BACKEND_KEYBOARD_RIGHT;
	conf->bindings[BINDING_OK].keyboard = BACKEND_KEYBOARD_Z;
	conf->bindings[BINDING_CANCEL].keyboard = BACKEND_KEYBOARD_X;
	conf->bindings[BINDING_JUMP].keyboard = BACKEND_KEYBOARD_Z;
	conf->bindings[BINDING_SHOT].keyboard = BACKEND_KEYBOARD_X;
	conf->bindings[BINDING_ARMSREV].keyboard = BACKEND_KEYBOARD_A;
	conf->bindings[BINDING_ARMS].keyboard = BACKEND_KEYBOARD_S;
	conf->bindings[BINDING_ITEM].keyboard = BACKEND_KEYBOARD_Q;
	conf->bindings[BINDING_MAP].keyboard = BACKEND_KEYBOARD_W;
	conf->bindings[BINDING_PAUSE].keyboard = BACKEND_KEYBOARD_ESCAPE;
}
