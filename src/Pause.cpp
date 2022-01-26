
#include "Pause.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Backends/Controller.h"
#include "Backends/Misc.h"
#include "Back.h"
#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "Game.h"
#include "Profile.h"
#include "Escape.h"
#include "Frame.h"
#include "Generic.h"
#include "KeyControl.h"
#include "Main.h"
#include "Organya.h"
#include "Sound.h"
#include "Stage.h"
#include "MyChar.h"

#define MAX_OPTIONS ((WINDOW_HEIGHT / 20) - 4)	// The maximum number of options we can fit on-screen at once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

enum
{
	CALLBACK_CONTINUE = -1,
	CALLBACK_PREVIOUS_MENU = -2,
	CALLBACK_RESET = -3,
	CALLBACK_EXIT = -4,
};

typedef enum CallbackAction
{
	ACTION_INIT,
	ACTION_DEINIT,
	ACTION_UPDATE,
	ACTION_OK,
	ACTION_LEFT,
	ACTION_RIGHT
} CallbackAction;

typedef struct Option
{
	const char *name;
	int (*callback)(struct OptionsMenu *parent_menu, size_t this_option, CallbackAction action);
	void *user_data;
	const char *value_string;
	long value;
	BOOL disabled;
} Option;

typedef struct OptionsMenu
{
	const char *title;
	const char *subtitle;
	struct Option *options;
	size_t total_options;
	int x_offset;
	BOOL submenu;
} OptionsMenu;

static BOOL restart_required;
unsigned int bg = 0;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

bool saving = false;

RECT pauseView = {0,0,WINDOW_WIDTH, WINDOW_HEIGHT};

static const char* GetKeyName(int key)
{
	switch (key)
	{
		case BACKEND_KEYBOARD_A:
			return "A";

		case BACKEND_KEYBOARD_B:
			return "B";

		case BACKEND_KEYBOARD_C:
			return "C";

		case BACKEND_KEYBOARD_D:
			return "D";

		case BACKEND_KEYBOARD_E:
			return "E";

		case BACKEND_KEYBOARD_F:
			return "F";

		case BACKEND_KEYBOARD_G:
			return "G";

		case BACKEND_KEYBOARD_H:
			return "H";

		case BACKEND_KEYBOARD_I:
			return "I";

		case BACKEND_KEYBOARD_J:
			return "J";

		case BACKEND_KEYBOARD_K:
			return "K";

		case BACKEND_KEYBOARD_L:
			return "L";

		case BACKEND_KEYBOARD_M:
			return "M";

		case BACKEND_KEYBOARD_N:
			return "N";

		case BACKEND_KEYBOARD_O:
			return "O";

		case BACKEND_KEYBOARD_P:
			return "P";

		case BACKEND_KEYBOARD_Q:
			return "Q";

		case BACKEND_KEYBOARD_R:
			return "R";

		case BACKEND_KEYBOARD_S:
			return "S";

		case BACKEND_KEYBOARD_T:
			return "T";

		case BACKEND_KEYBOARD_U:
			return "U";

		case BACKEND_KEYBOARD_V:
			return "V";

		case BACKEND_KEYBOARD_W:
			return "W";

		case BACKEND_KEYBOARD_X:
			return "X";

		case BACKEND_KEYBOARD_Y:
			return "Y";

		case BACKEND_KEYBOARD_Z:
			return "Z";

		case BACKEND_KEYBOARD_0:
			return "0";

		case BACKEND_KEYBOARD_1:
			return "1";

		case BACKEND_KEYBOARD_2:
			return "2";

		case BACKEND_KEYBOARD_3:
			return "3";

		case BACKEND_KEYBOARD_4:
			return "4";

		case BACKEND_KEYBOARD_5:
			return "5";

		case BACKEND_KEYBOARD_6:
			return "6";

		case BACKEND_KEYBOARD_7:
			return "7";

		case BACKEND_KEYBOARD_8:
			return "8";

		case BACKEND_KEYBOARD_9:
			return "9";

		case BACKEND_KEYBOARD_F1:
			return "F1";

		case BACKEND_KEYBOARD_F2:
			return "F2";

		case BACKEND_KEYBOARD_F3:
			return "F3";

		case BACKEND_KEYBOARD_F4:
			return "F4";

		case BACKEND_KEYBOARD_F5:
			return "F5";

		case BACKEND_KEYBOARD_F6:
			return "F6";

		case BACKEND_KEYBOARD_F7:
			return "F7";

		case BACKEND_KEYBOARD_F8:
			return "F8";

		case BACKEND_KEYBOARD_F9:
			return "F9";

		case BACKEND_KEYBOARD_F10:
			return "F10";

		case BACKEND_KEYBOARD_F11:
			return "F11";

		case BACKEND_KEYBOARD_F12:
			return "F12";

		case BACKEND_KEYBOARD_UP:
			return "Up";

		case BACKEND_KEYBOARD_DOWN:
			return "Down";

		case BACKEND_KEYBOARD_LEFT:
			return "Left";

		case BACKEND_KEYBOARD_RIGHT:
			return "Right";

		case BACKEND_KEYBOARD_ESCAPE:
			return "Escape";

		case BACKEND_KEYBOARD_BACK_QUOTE:
			return "`";

		case BACKEND_KEYBOARD_TAB:
			return "Tab";

		case BACKEND_KEYBOARD_CAPS_LOCK:
			return "Caps Lock";

		case BACKEND_KEYBOARD_LEFT_SHIFT:
			return "Left Shift";

		case BACKEND_KEYBOARD_LEFT_CTRL:
			return "Left Ctrl";

		case BACKEND_KEYBOARD_LEFT_ALT:
			return "Left Alt";

		case BACKEND_KEYBOARD_SPACE:
			return "Space";

		case BACKEND_KEYBOARD_RIGHT_ALT:
			return "Right Alt";

		case BACKEND_KEYBOARD_RIGHT_CTRL:
			return "Right Ctrl";

		case BACKEND_KEYBOARD_RIGHT_SHIFT:
			return "Right Shift";

		case BACKEND_KEYBOARD_ENTER:
			return "Enter";

		case BACKEND_KEYBOARD_BACKSPACE:
			return "Backspace";

		case BACKEND_KEYBOARD_MINUS:
			return "-";

		case BACKEND_KEYBOARD_EQUALS:
			return "=";

		case BACKEND_KEYBOARD_LEFT_BRACKET:
			return "[";

		case BACKEND_KEYBOARD_RIGHT_BRACKET:
			return "]";

		case BACKEND_KEYBOARD_BACK_SLASH:
			return "\\";

		case BACKEND_KEYBOARD_SEMICOLON:
			return ";";

		case BACKEND_KEYBOARD_APOSTROPHE:
			return "'";

		case BACKEND_KEYBOARD_COMMA:
			return ",";

		case BACKEND_KEYBOARD_PERIOD:
			return ".";

		case BACKEND_KEYBOARD_FORWARD_SLASH:
			return "/";
	}

	return "Unknown";
}

static void PutTextCentred(int x, int y, const char *text, unsigned long color)
{
	size_t string_width = 0;
	size_t string_height = font_height;

	for (const char *text_pointer = text; *text_pointer != '\0';)
	{
		if (IsShiftJIS(*text_pointer))
		{
			text_pointer += 2;
			string_width += font_width * 2;
		}
		else
		{
			text_pointer += 1;
			string_width += font_width;
		}
	}

	PutText(x - string_width / 4, y - string_height / 2, text, color, true);
}

static int EnterOptionsMenu(OptionsMenu *options_menu, size_t selected_option, bool ass = false)
{
	int scroll = 0;

	unsigned int anime = 0;

	int return_value;

	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	SetAltFont("monogram");

	// Initialise options
	for (size_t i = 0; i < options_menu->total_options; ++i)
		options_menu->options[i].callback(options_menu, i, ACTION_INIT);

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		// Allow unpausing by pressing the pause button only when in the main pause menu (not submenus)
		if (!options_menu->submenu && gKeyTrg & KEY_PAUSE)
		{
			return_value = CALLBACK_CONTINUE;
			break;
		}

		// Go back one submenu when the 'cancel' button is pressed
		if (gKeyTrg & gKeyCancel)
		{
			return_value = CALLBACK_CONTINUE;
			break;
		}

		// Handling up/down input
		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			const size_t old_selection = selected_option;

			if (gKeyTrg & gKeyDown)
				if (selected_option++ == options_menu->total_options - 1)
					selected_option = 0;

			if (gKeyTrg & gKeyUp)
				if (selected_option-- == 0)
					selected_option = options_menu->total_options - 1;

			// Update the menu-scrolling, if there are more options than can be fit on the screen
			if (selected_option < old_selection)
				scroll = MAX(0, MIN(scroll, (int)selected_option - 1));

			if (selected_option > old_selection)
				scroll = MIN(MAX(0, (int)options_menu->total_options - MAX_OPTIONS), MAX(scroll, (int)selected_option - (MAX_OPTIONS - 2)));

			PlaySoundObject(1, SOUND_MODE_PLAY);
		}

		// Run option callbacks
		for (size_t i = 0; i < options_menu->total_options; ++i)
		{
			if (!options_menu->options[i].disabled)
			{
				CallbackAction action = ACTION_UPDATE;

				if (i == selected_option)
				{
					if (gKeyTrg & gKeyOk)
						action = ACTION_OK;
					else if (gKeyTrg & gKeyLeft)
						action = ACTION_LEFT;
					else if (gKeyTrg & gKeyRight)
						action = ACTION_RIGHT;
				}

				return_value = options_menu->options[i].callback(options_menu, i, action);

				// If the callback returned something other than CALLBACK_CONTINUE, it's time to exit this submenu
				if (return_value != CALLBACK_CONTINUE)
					break;
			}
		}

		if (return_value != CALLBACK_CONTINUE)
			break;

		// Update Quote animation counter
		if (++anime >= 40)
			anime = 0;
		if (bg == 1) {

			std::string path;
			int frame_x;
			int frame_y;

			path = gTMT[53].back;

			InitBack(path.c_str(), gTMT[53].bkType);
			GetFramePosition(&frame_x, &frame_y);

			// Draw screen
			// Draw moon background
			ActBack();
			PutBack(frame_x, frame_y);
			RECT optionRect = {0, 0, 320, 240};
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 160), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 120), &optionRect, SURFACE_ID_OPTIOM);
		} else {
			PutBitmap4(&grcGame, 0, 0, &pauseView, SURFACE_ID_SCREEN_GRAB);
			PutBitmap3(&rcView, 0, 0, &pauseView, SURFACE_ID_MENU_OVERLAY);
		}
		
		// CortBox(&grcFull, 0x000000);

		const size_t visible_options = MIN(MAX_OPTIONS, options_menu->total_options);

		int y = (WINDOW_HEIGHT / 2) - ((visible_options * 20) / 2) - (40 / 2);

		// Draw title
		PutTextCentred(WINDOW_WIDTH / 2, y, options_menu->title, RGB(0xFF, 0xFF, 0xFF));

		// Draw subtitle
		if (options_menu->subtitle != NULL)
			PutTextCentred(WINDOW_WIDTH / 2, y + 14, options_menu->subtitle, RGB(0xFF, 0xFF, 0xFF));

		y += 40;

		for (size_t i = scroll; i < scroll + visible_options; ++i)
		{
			const int x = (WINDOW_WIDTH / 2) + options_menu->x_offset;

			// Draw Quote next to the selected option
			if (i == selected_option)
				PutBitmap3(&grcFull, PixelToScreenCoord(x - 20), PixelToScreenCoord(y - 8), &rcMyChar[anime / 10 % 4], SURFACE_ID_MY_CHAR);

			unsigned long option_colour = options_menu->options[i].disabled ? RGB(0x80, 0x80, 0x80) : RGB(0xFF, 0xFF, 0xFF);
			
			// Draw option name
			PutText(x, y - font_height / 2, options_menu->options[i].name, option_colour, true);

			// Draw option value, if it has one
			if (options_menu->options[i].value_string != NULL)
				PutText(x + 100, y - font_height / 2, options_menu->options[i].value_string, option_colour, true);

			y += 20;
		}

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			return_value = CALLBACK_EXIT;
			break;
		}
	}

	// Deinitialise options
	for (size_t i = 0; i < options_menu->total_options; ++i)
		options_menu->options[i].callback(options_menu, i, ACTION_DEINIT);

	return return_value;
}

///////////////////
// Controls menu //
///////////////////

typedef struct Control
{
	const char *name;
	size_t binding_index;
	unsigned char groups;
} Control;

// The bitfield on the right determines which 'group' the
// control belongs to - if two controls are in the same group,
// they cannot be bound to the same key.
static const Control controls[] = {
	{"Up",              BINDING_UP,     (1 << 0) | (1 << 1)},
	{"Down",            BINDING_DOWN,   (1 << 0) | (1 << 1)},
	{"Left",            BINDING_LEFT,   (1 << 0) | (1 << 1)},
	{"Right",           BINDING_RIGHT,  (1 << 0) | (1 << 1)},
	{"OK",              BINDING_OK,      1 << 1},
	{"Cancel",          BINDING_CANCEL,  1 << 1},
	{"Jump",            BINDING_JUMP,    1 << 0},
	{"Shoot",           BINDING_SHOT,    1 << 0},
	{"Next Weapon",     BINDING_ARMS,    1 << 0},
	{"Previous Weapon", BINDING_ARMSREV, 1 << 0},
	{"Inventory",       BINDING_ITEM,    1 << 0},
	{"Map",             BINDING_MAP,     1 << 0},
	{"Pause",           BINDING_PAUSE,   1 << 0}
};

static char bound_name_buffers[sizeof(controls) / sizeof(controls[0])][20];

static int Callback_ControlsKeyboard_Rebind(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	switch (action)
	{
		default:
			break;

		case ACTION_INIT:
			strncpy(bound_name_buffers[this_option], GetKeyName(bindings[controls[this_option].binding_index].keyboard), sizeof(bound_name_buffers[0]) - 1);
			break;

		case ACTION_OK:
			PlaySoundObject(5, SOUND_MODE_PLAY);

			char timeout_string[2];
			timeout_string[1] = '\0';

			bool previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];
			memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

			// Time-out and exit if the user takes too long (they probably don't want to rebind)
			for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
			{
				for (int scancode = 0; scancode < BACKEND_KEYBOARD_TOTAL; ++scancode)
				{
					// Wait for user to press a key
					if (!previous_keyboard_state[scancode] && gKeyboardState[scancode])
					{
						const char *key_name = GetKeyName(scancode);

						// If another control in the same group uses this key, swap them
						for (size_t other_option = 0; other_option < parent_menu->total_options; ++other_option)
						{
							if (other_option != this_option && controls[other_option].groups & controls[this_option].groups && bindings[controls[other_option].binding_index].keyboard == scancode)
							{
								bindings[controls[other_option].binding_index].keyboard = bindings[controls[this_option].binding_index].keyboard;
								memcpy(bound_name_buffers[other_option], bound_name_buffers[this_option], sizeof(bound_name_buffers[0]));
								break;
							}
						}

						// Otherwise just overwrite the selected control
						bindings[controls[this_option].binding_index].keyboard = scancode;
						strncpy(bound_name_buffers[this_option], key_name, sizeof(bound_name_buffers[0]) - 1);

						PlaySoundObject(18, SOUND_MODE_PLAY);

						gKeyTrg = gKey = 0;	// Prevent weird input-ghosting by doing this
						return CALLBACK_CONTINUE;
					}
				}

				memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

				// Draw screen
				// CortBox(&grcFull, 0x000000);

				const char *string = "Press a key to bind to this action:";
				PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 10, string, RGB(0xFF, 0xFF, 0xFF));
				PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 10, parent_menu->options[this_option].name, RGB(0xFF, 0xFF, 0xFF));

				timeout_string[0] = '0' + (timeout / 60) + 1;
				PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

				PutFramePerSecound();

				if (!Flip_SystemTask())
				{
					// Quit if window is closed
					return CALLBACK_EXIT;
				}
			}

			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_ControlsKeyboard(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	Option options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		options[i].name = controls[i].name;
		options[i].callback = Callback_ControlsKeyboard_Rebind;
		options[i].value_string = bound_name_buffers[i];
		options[i].disabled = FALSE;
	}

	OptionsMenu options_menu = {
		"CONTROLS (KEYBOARD)",
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-60,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	PlaySoundObject(5, SOUND_MODE_PLAY);

	return return_value;
}

static int Callback_ControlsController_Rebind(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	switch (action)
	{
		default:
			break;

		case ACTION_INIT:
			// Name each bound button
			if (bindings[controls[this_option].binding_index].controller == 0xFF)
				strncpy(bound_name_buffers[this_option], "[Unbound]", sizeof(bound_name_buffers[0]));
			else
				strncpy(bound_name_buffers[this_option], ControllerBackend_GetButtonName(bindings[controls[this_option].binding_index].controller), sizeof(bound_name_buffers[0]));

			break;

		case ACTION_OK:
			PlaySoundObject(5, SOUND_MODE_PLAY);

			DIRECTINPUTSTATUS old_state = gJoystickState;

			char timeout_string[2];
			timeout_string[1] = '\0';

			// Time-out and exit if the user takes too long (they probably don't want to rebind)
			for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
			{
				for (int button = 0; button < sizeof(gJoystickState.bButton) / sizeof(gJoystickState.bButton[0]); ++button)
				{
					// Wait for user to press a button
					if (!old_state.bButton[button] && gJoystickState.bButton[button])
					{
						// If another control in the same group uses this button, swap them
						for (size_t other_option = 0; other_option < parent_menu->total_options; ++other_option)
						{
							if (other_option != this_option && controls[other_option].groups & controls[this_option].groups && bindings[controls[other_option].binding_index].controller == button)
							{
								bindings[controls[other_option].binding_index].controller = bindings[controls[this_option].binding_index].controller;
								memcpy(bound_name_buffers[other_option], bound_name_buffers[this_option], sizeof(bound_name_buffers[0]));
								break;
							}
						}

						// Otherwise just overwrite the selected control
						bindings[controls[this_option].binding_index].controller = button;
						strncpy(bound_name_buffers[this_option], ControllerBackend_GetButtonName(button), sizeof(bound_name_buffers[0]));

						PlaySoundObject(18, SOUND_MODE_PLAY);

						gKeyTrg = gKey = 0;	// Prevent weird input-ghosting by doing this
						return CALLBACK_CONTINUE;
					}
				}

				old_state = gJoystickState;

				// Draw screen
				// CortBox(&grcFull, 0x000000);

				const char *string = "Press a button to bind to this action:";
				PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 10, string, RGB(0xFF, 0xFF, 0xFF));
				PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 10, parent_menu->options[this_option].name, RGB(0xFF, 0xFF, 0xFF));

				timeout_string[0] = '0' + (timeout / 60) + 1;
				PutTextCentred(WINDOW_WIDTH, WINDOW_HEIGHT / 2 + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

				PutFramePerSecound();

				if (!Flip_SystemTask())
				{
					// Quit if window is closed
					return CALLBACK_EXIT;
				}
			}

			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_ControlsController(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	Option options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		options[i].name = controls[i].name;
		options[i].callback = Callback_ControlsController_Rebind;
		options[i].value_string = bound_name_buffers[i];
		options[i].disabled = FALSE;
	}

	OptionsMenu options_menu = {
	#if !defined(__WIIU__) && !defined(_3DS)
		"CONTROLS (GAMEPAD)",
	#else
		"CONTROLS",
	#endif
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-70,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	PlaySoundObject(5, SOUND_MODE_PLAY);

	return return_value;
}

/////////////////////
// Soundtrack menu //
/////////////////////

static int Callback_Soundtrack_Option(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	switch (action)
	{
		default:
			break;

		case ACTION_INIT:
			parent_menu->options[this_option].disabled = !CheckSoundtrackExists(this_option);
			break;

		case ACTION_OK:
			conf->soundtrack = gSoundtrack = this_option;

			// Restart the songs, with the new soundtrack setting
			MusicID current_song = gMusicNo;
			MusicID previous_song = gOldNo;
			gMusicNo = gOldNo = MUS_SILENCE;
			ChangeMusic(previous_song);
			ChangeMusic(current_song);

			return CALLBACK_PREVIOUS_MENU;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Soundtrack(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	const char *strings[] = {"Organya", "New", "Remastered", "Famitracks", "Ridiculon", "SNES", "Arranged"};

	Option options[sizeof(strings) / sizeof(strings[0])];

	for (size_t i = 0; i < sizeof(strings) / sizeof(strings[0]); ++i)
	{
		options[i].name = strings[i];
		options[i].callback = Callback_Soundtrack_Option;
		options[i].user_data = conf;
		options[i].value_string = NULL;
	}

	OptionsMenu options_menu = {
		"SELECT SOUNDTRACK",
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-20,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	int return_value = EnterOptionsMenu(&options_menu, conf->soundtrack);

	// Check if we just want to go back to the previous menu
	if (return_value == CALLBACK_PREVIOUS_MENU)
	{
		return_value = CALLBACK_CONTINUE;

		PlaySoundObject(18, SOUND_MODE_PLAY);
	}
	else
	{
		PlaySoundObject(5, SOUND_MODE_PLAY);
	}

	return return_value;
}

//////////////////
// Options menu //
//////////////////

static int Callback_Framerate(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"50FPS", "60FPS"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->b60fps;
			parent_menu->options[this_option].value_string = strings[conf->b60fps];
			break;

		case ACTION_DEINIT:
			conf->b60fps = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			gb60fps = parent_menu->options[this_option].value;

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Autojump(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Off", "On"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->bAutojump;
			parent_menu->options[this_option].value_string = strings[conf->bAutojump];
			break;

		case ACTION_DEINIT:
			conf->bAutojump = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			gbAutojump = parent_menu->options[this_option].value;

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_AltText(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Off", "On"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->bAltText;
			parent_menu->options[this_option].value_string = strings[conf->bAltText];
			break;

		case ACTION_DEINIT:
			conf->bAltText = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			gbFR = parent_menu->options[this_option].value;

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_NoDamage(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Off", "On"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->bNoDamage;
			parent_menu->options[this_option].value_string = strings[conf->bNoDamage];
			break;

		case ACTION_DEINIT:
			conf->bNoDamage = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			gbNoDamage = parent_menu->options[this_option].value;

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Vsync(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Off", "On"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->bVsync;
			parent_menu->options[this_option].value_string = strings[conf->bVsync];
			break;

		case ACTION_DEINIT:
			conf->bVsync = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			restart_required = TRUE;
			parent_menu->subtitle = "RESTART REQUIRED";

			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Resolution(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Fullscreen", "Windowed 426x240", "Windowed 852x480", "Windowed 1278x720", "Windowed 1704x960"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->display_mode;
			parent_menu->options[this_option].value_string = strings[conf->display_mode];
			break;

		case ACTION_DEINIT:
			conf->display_mode = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			restart_required = TRUE;
			parent_menu->subtitle = "RESTART REQUIRED";

			if (action == ACTION_LEFT)
			{
				// Decrement value (with wrapping)
				if (--parent_menu->options[this_option].value < 0)
					parent_menu->options[this_option].value = (sizeof(strings) / sizeof(strings[0])) - 1;
			}
			else
			{
				// Increment value (with wrapping)
				if (++parent_menu->options[this_option].value > (sizeof(strings) / sizeof(strings[0])) - 1)
					parent_menu->options[this_option].value = 0;
			}

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_SmoothScrolling(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	CONFIGDATA *conf = (CONFIGDATA*)parent_menu->options[this_option].user_data;

	const char *strings[] = {"Off", "On"};

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->bSmoothScrolling;
			parent_menu->options[this_option].value_string = strings[conf->bSmoothScrolling];
			break;

		case ACTION_DEINIT:
			conf->bSmoothScrolling = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			// Increment value (with wrapping)
			parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

			gbSmoothScrolling = parent_menu->options[this_option].value;

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Options(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	// Make the options match the configuration data
	static bool config_loaded = false;
	static CONFIGDATA conf;

	if (!config_loaded)
	{
		config_loaded = true;

		if (!LoadConfigData(&conf))
			DefaultConfigData(&conf);
	}

	const CONFIGDATA previous_conf = conf;

	Option options[] = {
	#if !defined(__WIIU__) && !defined(_3DS)
		{"Controls (Keyboard)", Callback_ControlsKeyboard, NULL, NULL, 0, FALSE},
		{"Controls (Gamepad)", Callback_ControlsController, NULL, NULL, 0, FALSE},
	#else
		{"Controls", Callback_ControlsController, NULL, NULL, 0, FALSE},
	#endif

		// {"Soundtrack", Callback_Soundtrack, &conf, NULL, 0, FALSE}, removed until further notice, lmfao
		{"Framerate", Callback_Framerate, &conf, NULL, 0, FALSE},

	#if !defined(__WIIU__) && !defined(_3DS)
		{"V-sync", Callback_Vsync, &conf, NULL, 0, FALSE},
		{"Resolution", Callback_Resolution, &conf, NULL, 0, FALSE},
	#endif

	#if !defined(_3DS)
		{"Smooth Scrolling", Callback_SmoothScrolling, &conf, NULL, 0, FALSE},
	#endif
		{"Autojump", Callback_Autojump, &conf, NULL, 0, FALSE},
		{"Disable Damage", Callback_NoDamage, &conf, NULL, 0, FALSE},
		{"Use Alt Text", Callback_AltText, &conf, NULL, 0, FALSE},
	};

	OptionsMenu options_menu = {
		"OPTIONS",
		restart_required ? "RESTART REQUIRED" : NULL,
		options,
		sizeof(options) / sizeof(options[0]),
	#if !defined(__WIIU__) && !defined(_3DS)
		-70,
	#else
		-60,
	#endif
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	PlaySoundObject(5, SOUND_MODE_PLAY);

	// Save our changes to the configuration file
	memcpy(conf.bindings, bindings, sizeof(bindings));

	if (memcmp(&conf, &previous_conf, sizeof(CONFIGDATA)))
	{
		// Draw 'saving' prompt
		// CortBox(&grcFull, 0x000000);
		PutTextCentred(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Saving...", RGB(0xFF, 0xFF, 0xFF));
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return CALLBACK_EXIT;

		SaveConfigData(&conf);
	}

	return return_value;
}

////////////////
// Pause menu //
////////////////

static int PromptAreYouSure(void)
{
	struct FunctionHolder
	{
		static int Callback_Yes(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
		{
			(void)parent_menu;
			(void)this_option;

			if (action != ACTION_OK)
				return CALLBACK_CONTINUE;

			return 1;	// Yes
		}

		static int Callback_No(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
		{
			(void)parent_menu;
			(void)this_option;

			if (action != ACTION_OK)
				return CALLBACK_CONTINUE;

			return 0;	// No
		}
	};

	Option options[] = {
		{"Yes", FunctionHolder::Callback_Yes, NULL, NULL, 0, FALSE},
		{"No", FunctionHolder::Callback_No, NULL, NULL, 0, FALSE}
	};

	OptionsMenu options_menu = {
		"ARE YOU SURE?",
		"UNSAVED PROGRESS WILL BE LOST",
		options,
		sizeof(options) / sizeof(options[0]),
		-10,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	int return_value = EnterOptionsMenu(&options_menu, 1);

	PlaySoundObject(18, SOUND_MODE_PLAY);

	return return_value;
}

static int Callback_Resume(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	PlaySoundObject(18, SOUND_MODE_PLAY);
	return enum_ESCRETURN_continue;
}

static int Callback_Reset(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	int return_value = PromptAreYouSure();

	switch (return_value)
	{
		case 0:
			return_value = CALLBACK_CONTINUE;	// Go back to previous menu
			break;

		case 1:
			return_value = CALLBACK_RESET;	// Restart game
			break;
	}

	return return_value;
}

static int Callback_Reload(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	int return_value = PromptAreYouSure();

	switch (return_value)
	{
		case 0:
			return_value = CALLBACK_CONTINUE;	// Go back to previous menu
			break;

		case 1:
			ChangeMusic(MUS_SILENCE);
			return_value = -5;
			break;
	}

	return return_value;
}

static int Callback_Quit(OptionsMenu *parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	int return_value = PromptAreYouSure();

	switch (return_value)
	{
		case 0:
			return_value = CALLBACK_CONTINUE;	// Go back to previous menu
			break;

		case 1:
			return_value = CALLBACK_EXIT;	// Exit game
			break;
	}

	return return_value;
}

int Call_Pause(void)
{
	Option options[] = {
		{"Resume", Callback_Resume, NULL, NULL, 0, FALSE},
		{"Reset", Callback_Reset, NULL, NULL, 0, FALSE},
		{"Reload Session", Callback_Reload, NULL, NULL, 0, FALSE},
		{"Options", Callback_Options, NULL, NULL, 0, FALSE},
		{"Quit", Callback_Quit, NULL, NULL, 0, FALSE}
	};

	OptionsMenu options_menu = {
		"PAUSED",
		NULL,
		options,
		sizeof(options) / sizeof(options[0]),
		-14,
		FALSE
	};

	bg = 0;

	BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);

	int return_value = EnterOptionsMenu(&options_menu, 0, true);

	// Filter internal return values to something Cave Story can understand
	switch (return_value)
	{
		case CALLBACK_CONTINUE:
			return_value = enum_ESCRETURN_continue;
			bg = 0;
			break;

		case CALLBACK_RESET:
			return_value = enum_ESCRETURN_restart;
			bg = 0;
			break;

		case CALLBACK_EXIT:
			return_value = enum_ESCRETURN_exit;
			bg = 0;
			break;

		case -5:
			return_value = enum_ESCRETURN_restart2;
			bg = 0;
			break;
	}

	gKeyTrg = gKey = 0;	// Avoid input-ghosting

	return return_value;
}

int Call_Options(void)
{
	// Make the options match the configuration data
	CONFIGDATA conf;
	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	Option options[] = {
	#if !defined(__WIIU__) && !defined(_3DS)
		{"Controls (Keyboard)", Callback_ControlsKeyboard, NULL, NULL, 0, FALSE},
		{"Controls (Gamepad)", Callback_ControlsController, NULL, NULL, 0, FALSE},
	#else
		{"Controls", Callback_ControlsController, NULL, NULL, 0, FALSE},
	#endif

		// {"Soundtrack", Callback_Soundtrack, &conf, NULL, 0, FALSE},
		{"Framerate", Callback_Framerate, &conf, NULL, 0, FALSE},

	#if !defined(__WIIU__) && !defined(_3DS)
		{"V-sync", Callback_Vsync, &conf, NULL, 0, FALSE},
		{"Resolution", Callback_Resolution, &conf, NULL, 0, FALSE},
	#endif

	#if !defined(_3DS)
		{"Smooth Scrolling", Callback_SmoothScrolling, &conf, NULL, 0, FALSE},
	#endif
		{"Autojump", Callback_Autojump, &conf, NULL, 0, FALSE},
		{"Disable Damage", Callback_NoDamage, &conf, NULL, 0, FALSE},
		{"Use Alt Text", Callback_AltText, &conf, NULL, 0, FALSE},
	};

	
	OptionsMenu options_menu = {
		"OPTIONS",
		restart_required ? "RESTART REQUIRED" : NULL,
		options,
		(sizeof(options) / sizeof(options[0])),
		-90,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	bg = 1;

	int return_value = EnterOptionsMenu(&options_menu, 0);

	PlaySoundObject(18, SOUND_MODE_PLAY);

	// Save our changes to the configuration file
	memcpy(conf.bindings, bindings, sizeof(bindings));

	SaveConfigData(&conf);

	// Filter internal return values to something Cave Story can understand
	switch (return_value)
	{
		case CALLBACK_CONTINUE:
			return_value = enum_ESCRETURN_continue;
			bg = 0;
			break;

		case CALLBACK_RESET:
			return_value = enum_ESCRETURN_restart;
			bg = 0;
			break;

		case CALLBACK_EXIT:
			return_value = 0;
			bg = 0;
			break;

		case 3:
			return_value = enum_ESCRETURN_restart2;
			bg = 0;
			break;
	}

	gKeyTrg = gKey = 0;	// Avoid input-ghosting

	return return_value;
}

int lastCounter = 0;

int Call_SaveMenu(void)
{
	PROFILEDATA prof;
	PROFILEDATA profs[] = {LoadProfileSimple(1),LoadProfileSimple(2),LoadProfileSimple(3),LoadProfileSimple(4),LoadProfileSimple(5)};
	// Set rects
	RECT rcSaveBox = {240, 0, 340, 64+16};
	RECT rcSaveCur = {0, 0, 217, 24};
	RECT rcDataBox = {0, 0, 244, 40};
	RECT rcNOSAVEBox = {0, 40, 244, 40*2};
	RECT rcNewSaveBox = {0, 0, 320, 240};

	RECT rcMyChar = {0, 16, 16, 32};

	// save shit
	RECT rcCase = {0, 40, 232, 48};
	RECT rcLife = {0, 24, 232, 32};
	RECT rcBr = {0, 32, 232, 40};

	unsigned int wait;

	int anime;
	int v1, v2, v3, v4;

	RECT char_rc;
	int char_type;
	int time_counter;
	int char_y;
	int frame_x;
	int frame_y;
	SurfaceID char_surf;
	unsigned long back_color;

	// Create variables
	anime = 0;
	char_type = 0;
	time_counter = 0;
	back_color = GetCortBoxColor(RGB(0x20, 0x20, 0x20));

	lastCounter = sCounter;

	GetCompileVersion(&v1, &v2, &v3, &v4);

	// Reset cliprect, flags, and give the player the Nikumaru counter
	grcGame.left = 0;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Non-vanilla: these three lines are widescreen-related
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;
#endif

	g_GameFlags = 0;

	// Start loop
	wait = 0;

	BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);

	SetAltFont("monogram");

	prof = LoadProfileSimple(sCounter + 1);

	while (1)
	{
		// Get pressed keys
		GetTrg();

		if (gKeyTrg & gKeyOk)
		{
			PlaySoundObject(18, SOUND_MODE_PLAY);
			return 1337;
		}

		// Move cursor
		if (gKeyTrg & gKeyUp)
		{
			PlaySoundObject(1, SOUND_MODE_PLAY);
			sCounter = sCounter - 1;
			if (sCounter < 0)
				sCounter = 0;
			else if (sCounter > 4)
				sCounter = 4;
			prof = LoadProfileSimple(sCounter + 1);
				
		}
		if (gKeyTrg & gKeyDown)
		{
			PlaySoundObject(1, SOUND_MODE_PLAY);
			sCounter = sCounter + 1;
			if (sCounter < 0)
				sCounter = 0;
			else if (sCounter > 4)
				sCounter = 4;
			prof = LoadProfileSimple(sCounter + 1);
		}

		if (gKeyTrg & KEY_SHOT) {
			sCounter = lastCounter;
			return 6;
		}

		// Animate character cursor
		if (++anime >= 40)
			anime = 0;

		// Draw title
		// CortBox(&grcGame, back_color);

		PutBitmap4(&grcGame, 0, 0, &grcGame, SURFACE_ID_SCREEN_GRAB);

		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 160), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 120), &rcNewSaveBox, SURFACE_ID_BEEG_SAVE_BOX);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 121), PixelToScreenCoord(10), &rcDataBox, SURFACE_ID_SAVE_BOX);
		// PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 46), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 32), &rcSaveBox, SURFACE_ID_TITLE);

		char_rc = rcSaveCur;
		char_surf = SURFACE_ID_SAVE_CURSOR;

		switch (sCounter + 1) {
			case 1:
				char_y = 68;
				break;
			case 2:
				char_y = 104;
				break;
			case 3:
				char_y = 140;
				break;
			case 4:
				char_y = 176;
				break;
			case 5:
				char_y = 208;
				break;
		}

		// validate if data is a valid one
		if (prof.stage == -1) // prof.code doesnt work :V
		{
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 121), PixelToScreenCoord(10), &rcNOSAVEBox, SURFACE_ID_SAVE_BOX);
		}
		else {
			rcCase.right = 64;
			rcLife.right = ((prof.life * 40) / prof.max_life) - 1;
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 111), PixelToScreenCoord(20), &rcMyChar, SURFACE_ID_MY_CHAR);
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 83), PixelToScreenCoord(25), &rcCase, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 59), PixelToScreenCoord(25), &rcLife, SURFACE_ID_TEXT_BOX);
			
			PutNumber4((WINDOW_WIDTH / 2) - 91, 25, prof.life, FALSE);

			int x;
			int a;
			RECT rect = {0, 0, 0, 16};

			int arms_num = 0;
			
			while (prof.arms[arms_num].code != 0)
				++arms_num;

			if (arms_num != 0)
			{
				for (a = 0; a < arms_num; ++a)
				{
					// Get X position to draw at
					x = (a * 16) + 20;

					// Draw icon
					rect.left = prof.arms[a].code * 16;
					rect.right = rect.left + 16;
					PutBitmap3(&grcGame, PixelToScreenCoord(x) + PixelToScreenCoord((WINDOW_WIDTH / 2) - 35), PixelToScreenCoord(20), &rect, SURFACE_ID_ARMS_IMAGE);
				}
			}
		}

		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 107), PixelToScreenCoord(char_y - 6), &char_rc, char_surf);
		int location[6] = {68, 104, 140, 176, 208};
		int in = 0;
		for (int i = 0; i < 5; i++)
		{
			if (profs[i].stage != -1)
				PutText((WINDOW_WIDTH / 2) - 96, location[i] - 2, gTMT[profs[i].stage].name, RGB(0xFF, 0xFF, 0xFF), true);
			else
				PutText((WINDOW_WIDTH / 2) - 96, location[i] - 2, "NO DATA", RGB(0xFF, 0xFF, 0xFF), true);
		}

		if (!Flip_SystemTask())
			return 0;
	}

	return 1337;
}
