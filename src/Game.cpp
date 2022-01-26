// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include "Game.h"

#include <stddef.h>
#include <string>

#include "WindowsWrapper.h"
#include "Config.h"

#include "Backends/Misc.h"
#include "ArmsItem.h"
#include "Back.h"
#include "Boss.h"
#include "BossLife.h"
#include "BulHit.h"
#include "Bullet.h"
#include "Caret.h"
#include "CommonDefines.h"
#include "Draw.h"
#include "Ending.h"
#include "Escape.h"
#include "Fade.h"
#include "Flags.h"
#include "Flash.h"
#include "Frame.h"
#include "Generic.h"
#include "GenericLoad.h"
#include "KeyControl.h"
#include "Main.h"
#include "Map.h"
#include "MapName.h"
#include "MiniMap.h"
#include "MyChar.h"
#include "MycHit.h"
#include "MycParam.h"
#include "NpChar.h"
#include "NpcHit.h"
#include "NpcTbl.h"
#include "Pause.h"
#include "Profile.h"
#include "Random.h"
#include "SelStage.h"
#include "Shoot.h"
#include "Sound.h"
#include "Stage.h"
#include "Star.h"
#include "TextScr.h"
#include "ValueView.h"

int g_GameFlags;
int gCounter;

static BOOL bContinue;
static BOOL bOptions;
static int tCounter;
int sCounter;
int mode;
BOOL reloadN = false;
PROFILEDATA prof;
PROFILEDATA profs[] = {LoadProfileSimple(1),LoadProfileSimple(2),LoadProfileSimple(3),LoadProfileSimple(4),LoadProfileSimple(5)};

int Random(int min, int max)
{
	const int range = max - min + 1;
	return (msvc_rand() % range) + min;
}

void PutNumber4(int x, int y, int value, BOOL bZero)
{
	// Define rects
	RECT rcClient = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	RECT rect[10] = {
		{0, 56, 8, 64},
		{8, 56, 16, 64},
		{16, 56, 24, 64},
		{24, 56, 32, 64},
		{32, 56, 40, 64},
		{40, 56, 48, 64},
		{48, 56, 56, 64},
		{56, 56, 64, 64},
		{64, 56, 72, 64},
		{72, 56, 80, 64},
	};

	// Digits
	int tbl[4] = {1000, 100, 10, 1};

	int a;
	int sw;
	int offset;

	// Limit value
	if (value > 9999)
		value = 9999;

	// Go through number and draw digits
	offset = 0;
	sw = 0;
	while (offset < 4)
	{
		// Get the digit that this is
		a = 0;

		while (value >= tbl[offset])
		{
			value -= tbl[offset];
			++a;
			++sw;
		}

		// Draw digit
		if ((bZero && offset == 2) || sw != 0 || offset == 3)
			PutBitmap3(&rcClient, PixelToScreenCoord(x + 8 * offset), PixelToScreenCoord(y), &rect[a], SURFACE_ID_TEXT_BOX);

		// Go to next digit
		++offset;
	}
}

// Draws black bars to cover the out-of-bounds parts of the screen
static void PutBlackBars(int fx, int fy)
{
	RECT rect;

	// Don't draw if credits are running
	if (g_GameFlags & 8)
		return;

	int stage_left;
	int stage_top;
	int stage_width;
	int stage_height;

	// Force black-bars in the IronHead battle
	if (gStageNo == 31)
	{
		stage_left = 20 * 16;
		stage_top = 0;
		stage_width = 320;
		stage_height = 240;
	}
	else
	{
		stage_left = 0;
		stage_top = 0;
		stage_width = (gMap.width - 1) * 16;
		stage_height = (gMap.length - 1) * 16;
	}

	// Left bar
	rect.left = 0;
	rect.top = 0;
	rect.right = stage_left - (fx / 0x200);
	rect.bottom = WINDOW_HEIGHT;
	CortBox(&rect, 0);

	// Right bar
	rect.left = stage_left + stage_width - (fx / 0x200);
	rect.top = 0;
	rect.right = WINDOW_WIDTH;
	rect.bottom = WINDOW_HEIGHT;
	CortBox(&rect, 0);

	// Top bar
	rect.left = 0;
	rect.top = 0;
	rect.right = WINDOW_WIDTH;
	rect.bottom = stage_top - (fy / 0x200);
	CortBox(&rect, 0);

	// Bottom bar
	rect.left = 0;
	rect.top = stage_top + stage_height - (fy / 0x200);
	rect.right = WINDOW_WIDTH;
	rect.bottom = WINDOW_HEIGHT;
	CortBox(&rect, 0);
}

static int ModeOpening(void)
{
	int frame_x;
	int frame_y;
	unsigned int wait;

	InitNpChar();
	InitCaret();
	InitStar();
	InitFade();
	InitFlash();
	InitBossLife();
	ChangeMusic(MUS_SILENCE);
	TransferStage(72, 100, 3, 3);
	SetFrameTargetMyChar(16);
	SetFadeMask();

	// Reset cliprect and flags
	grcGame.left = 0;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Non-vanilla: these three lines are widescreen-related
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;
#endif

	g_GameFlags = 3;

	CutNoise();

	if (reloadN)
	{
		reloadN = false;
		return 4;
	}

	wait = 0;
	while (wait < 500)
	{
		// Increase timer
		++wait;

		// Get pressed keys
		GetTrg();

		if (gKey & KEY_PAUSE)
		{
			switch (Call_Pause())
			{
				case enum_ESCRETURN_exit:
					return 0;

				case enum_ESCRETURN_restart:
					return 1;
			}
		}

		// Escape menu
		if (gKey & KEY_ESCAPE)
		{
			switch (Call_Escape())
			{
				case enum_ESCRETURN_exit:
					return 0;

				case enum_ESCRETURN_restart:
					return 1;
			}
		}

		// Skip intro if OK is pressed
		if (gKey & gKeyOk)
			break;

		// Update everything
		ActNpChar();
		ActBossChar();
		ActBack();
		ResetMyCharFlag();
		HitMyCharMap();
		HitMyCharNpChar();
		HitMyCharBoss();
		HitNpCharMap();
		HitBossMap();
		HitBossBullet();
		ActCaret();
		MoveFrame3();
		ProcFade();

		// Draw everything
		CortBox(&grcFull, 0x000000);

		GetFramePosition(&frame_x, &frame_y);
		PutBack(frame_x, frame_y);
		PutStage_Back(frame_x, frame_y);
		PutBossChar(frame_x, frame_y);
		PutNpChar(frame_x, frame_y);
		PutMapDataVector(frame_x, frame_y);
		PutStage_Front(frame_x, frame_y);
		PutFront(frame_x, frame_y);
		PutBlackBars(frame_x, frame_y);
		PutCaret(frame_x, frame_y);
		PutFade();

		// Update Text Script
		switch (TextScriptProc())
		{
			case enum_ESCRETURN_exit:
				return 0;

			case enum_ESCRETURN_restart:
				return 1;
		}

		PutMapName(FALSE);
		PutTextScript();
		PutFramePerSecound();

		if (!Flip_SystemTask())
			return 0;

		++gCounter;
	}

	wait = Backend_GetTicks();
	while (Backend_GetTicks() < wait + 500)
	{
		CortBox(&grcGame, 0x000000);
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}
	return 2;
}

static int ModeTitle(void)
{
	// Set rects
	RECT rcTitle = {0, 0, 144, 40};
	RECT rcPixel = {0, 0, 160, 16};
	RECT rcNew = {144, 0, 210, 16};
	RECT rcContinue = {144, 16, 210, 32};
	RECT rcOptions = {144, 32, 210, 48};

	RECT rcVersion = {152, 80, 208, 88};
	RECT rcPeriod = {152, 88, 208, 96};

	RECT rcTitleBox = {0, 0, 108, 88};

	// Character rects
	RECT rcMyChar[4] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{0, 16, 16, 32},
		{32, 16, 48, 32},
	};

	RECT rcCurly[4] = {
		{0, 112, 16, 128},
		{16, 112, 32, 128},
		{0, 112, 16, 128},
		{32, 112, 48, 128},
	};

	RECT rcToroko[4] = {
		{64, 80, 80, 96},
		{80, 80, 96, 96},
		{64, 80, 80, 96},
		{96, 80, 112, 96},
	};

	RECT rcKing[4] = {
		{224, 48, 240, 64},
		{288, 48, 304, 64},
		{224, 48, 240, 64},
		{304, 48, 320, 64},
	};

	RECT rcSu[4] = {
		{0, 16, 16, 32},
		{32, 16, 48, 32},
		{0, 16, 16, 32},
		{48, 16, 64, 32},
	};

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

	// Reset everything
	InitCaret();
	InitStar();
	CutNoise();

	// Create variables
	anime = 0;
	char_type = 0;
	time_counter = 0;
	back_color = GetCortBoxColor(RGB(0x20, 0x20, 0x20));

	GetCompileVersion(&v1, &v2, &v3, &v4);

	// Set state
	if (IsProfile())
	{
		tCounter = 1;
		bContinue = TRUE;
	}
	else
	{
		tCounter = 0;
		bContinue = FALSE;
	}

	// Set character
	time_counter = LoadTimeCounter();

	if (time_counter && time_counter < 6 * 60 * (gb60fps ? 60 : 50))	// 6 minutes
		char_type = 1;
	if (time_counter && time_counter < 5 * 60 * (gb60fps ? 60 : 50))	// 5 minutes
		char_type = 2;
	if (time_counter && time_counter < 4 * 60 * (gb60fps ? 60 : 50))	// 4 minutes
		char_type = 3;
	if (time_counter && time_counter < 3 * 60 * (gb60fps ? 60 : 50))	// 3 minutes
		char_type = 4;

	// Set music to character's specific music
	if (char_type == 1)
		ChangeMusic(MUS_RUNNING_HELL);
	else if (char_type == 2)
		ChangeMusic(MUS_TOROKOS_THEME);
	else if (char_type == 3)
		ChangeMusic(MUS_WHITE);
	else if (char_type == 4)
		ChangeMusic(MUS_SAFETY);
	else
		ChangeMusic(MUS_CAVE_STORY);

	// Reset cliprect, flags, and give the player the Nikumaru counter
	grcGame.left = 0;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Non-vanilla: these three lines are widescreen-related
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;
#endif

	g_GameFlags = 0;
	gMC.equip |= EQUIP_NIKUMARU_COUNTER;	// Give the player the Nikumaru Counter so the timer appears on-screen

	std::string path;

	path = gTMT[53].back;

	InitBack(path.c_str(), gTMT[53].bkType);
	GetFramePosition(&frame_x, &frame_y);

	// Start loop
	wait = 0;

	while (1)
	{
		// Don't accept selection for 10 frames
		if (wait < 10)
			++wait;

		// Get pressed keys
		GetTrg();

		// Quit when OK is pressed
		if (wait >= 10)
		{
			if (gKeyTrg & gKeyOk)
			{
				if (tCounter != 2)
				{
					PlaySoundObject(18, SOUND_MODE_PLAY);
					break;
				}
				else {
					switch (Call_Options())
					{
						case enum_ESCRETURN_exit:
							return 0;

						case enum_ESCRETURN_restart:
							return 1;
					}
				}
			}
		}

		// Move cursor
		if (gKeyTrg & gKeyUp)
		{
			PlaySoundObject(1, SOUND_MODE_PLAY);
			tCounter = tCounter - 1;
				
		}
		if (gKeyTrg & gKeyDown)
		{
			PlaySoundObject(1, SOUND_MODE_PLAY);
			tCounter = tCounter + 1;
		}

		if (tCounter < 0)
			tCounter = 2;
		else if (tCounter > 2)
			tCounter = 0;

		// Update carets
		ActCaret();

		// Animate character cursor
		if (++anime >= 40)
			anime = 0;

		// Draw moon background
		ActBack();
		PutBack(frame_x, frame_y);

		// Draw title
		// CortBox(&grcGame, back_color);

		// Draw version
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 60), PixelToScreenCoord(WINDOW_HEIGHT - 24), &rcVersion, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 4), PixelToScreenCoord(WINDOW_HEIGHT - 24), &rcPeriod, SURFACE_ID_TEXT_BOX);

		PutNumber4((WINDOW_WIDTH / 2) - 20, WINDOW_HEIGHT - 24, v1, FALSE);
		PutNumber4((WINDOW_WIDTH / 2) - 4, WINDOW_HEIGHT - 24, v2, FALSE);
		PutNumber4((WINDOW_WIDTH / 2) + 12, WINDOW_HEIGHT - 24, v3, FALSE);
		PutNumber4((WINDOW_WIDTH / 2) + 28, WINDOW_HEIGHT - 24, v4, FALSE);

		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 54), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 24), &rcTitleBox, SURFACE_ID_TITLEM);

		// Draw main title
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 72), PixelToScreenCoord(40), &rcTitle, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 24), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 12), &rcNew, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 24), PixelToScreenCoord((WINDOW_HEIGHT / 2) + 8), &rcContinue, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 24), PixelToScreenCoord((WINDOW_HEIGHT / 2) + 30), &rcOptions, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 80), PixelToScreenCoord(WINDOW_HEIGHT - 54), &rcPixel, SURFACE_ID_PIXEL);
		PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 80), PixelToScreenCoord(WINDOW_HEIGHT - 42), &rcPixel, SURFACE_ID_MINUS);

		// Draw character cursor
		switch (char_type)
		{
			case 0:
				char_rc = rcMyChar[anime / 10 % 4];
				char_surf = SURFACE_ID_MY_CHAR;
				break;
			case 1:
				char_rc = rcCurly[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 2:
				char_rc = rcToroko[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 3:
				char_rc = rcKing[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 4:
				char_rc = rcSu[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
		}

		switch (tCounter) {
			case 0:
				char_y = (WINDOW_HEIGHT / 2) - 13;
				break;
			case 1:
				char_y = (WINDOW_HEIGHT / 2) + 7;
				break;
			case 2:
				char_y = (WINDOW_HEIGHT / 2) + 27;
				break;
		}

		// Pixel being redundant
		if (!bContinue)
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 44), PixelToScreenCoord(char_y), &char_rc, char_surf);
		else
			PutBitmap3(&grcGame, PixelToScreenCoord((WINDOW_WIDTH / 2) - 44), PixelToScreenCoord(char_y), &char_rc, char_surf);

		// Draw carets
		PutCaret(0, 0);

		if (time_counter)
			PutTimeCounter(16, 8);

		PutFramePerSecound();

		if (!Flip_SystemTask())
			return 0;
	}

	return 3;
}

static int ModeSaves(void) {
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

	// Reset everything
	InitCaret();
	InitStar();
	CutNoise();

	// Create variables
	anime = 0;
	char_type = 0;
	time_counter = 0;
	back_color = GetCortBoxColor(RGB(0x20, 0x20, 0x20));

	GetCompileVersion(&v1, &v2, &v3, &v4);

	sCounter = 0;

	prof = LoadProfileSimple(sCounter + 1);
	if (prof.stage == -1)
		ChangeMusic(MUS_CAVE_STORY);
	else
	 	ChangeMusic(prof.music);
	for (int i = 0; i < 5; i++)
	{
		profs[i] = LoadProfileSimple(i + 1);
	}

	// Reset cliprect, flags, and give the player the Nikumaru counter
	grcGame.left = 0;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Non-vanilla: these three lines are widescreen-related
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;
#endif

	g_GameFlags = 0;
	gMC.equip |= EQUIP_NIKUMARU_COUNTER;	// Give the player the Nikumaru Counter so the timer appears on-screen

	std::string path;

	path = gTMT[53].back;

	InitBack(path.c_str(), gTMT[53].bkType);
	GetFramePosition(&frame_x, &frame_y);

	SetAltFont("monogram");

	// Start loop
	wait = 0;

	while (1)
	{
		// Don't accept selection for 10 frames
		if (wait < 10)
			++wait;

		// Get pressed keys
		GetTrg();

		// Quit when OK is pressed
		if (wait >= 10)
		{
			if (gKeyTrg & gKeyOk)
			{
				PlaySoundObject(18, SOUND_MODE_PLAY);
				break;
			}
		}

		if (tCounter == 0) {
			break;
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
			if (prof.stage == -1)
				ChangeMusic(MUS_CAVE_STORY);
			else
			 	ChangeMusic(prof.music);
				
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
			if (prof.stage == -1)
				ChangeMusic(MUS_CAVE_STORY);
			else
			 	ChangeMusic(prof.music);
		}

		if (gKeyTrg & KEY_ESCAPE)
			return 2;

		// Update carets
		ActCaret();

		// Animate character cursor
		if (++anime >= 40)
			anime = 0;

		// Draw moon background
		ActBack();
		PutBack(frame_x, frame_y);

		// Draw title
		// CortBox(&grcGame, back_color);

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
		// this is where things get.... spicy
		int location[6] = {68, 104, 140, 176, 208};
		int in = 0;
		
		for (int i = 0; i < 5; i++)
		{
			if (profs[i].stage != -1)
				PutText((WINDOW_WIDTH / 2) - 96, location[i] - 2, gTMT[profs[i].stage].name, RGB(0xFF, 0xFF, 0xFF), true);
			else
				PutText((WINDOW_WIDTH / 2) - 96, location[i] - 2, "NO DATA", RGB(0xFF, 0xFF, 0xFF), true);
		}

		PutFramePerSecound();

		if (!Flip_SystemTask())
			return 0;
	}

	ChangeMusic(MUS_SILENCE);

	// Black screen when option is selected
	wait = Backend_GetTicks();
	while (Backend_GetTicks() < wait + 1000)
	{
		CortBox(&grcGame, 0);
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}

	return 4;
}

static int ModeAction(void)
{
	int frame_x;
	int frame_y;

	unsigned int swPlay;
	unsigned long color = GetCortBoxColor(RGB(0, 0, 0x20));

	swPlay = 1;

	// Reset stuff
	gCounter = 0;
	grcGame.left = 0;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Non-vanilla: these three lines are widescreen-related
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;
#endif
	g_GameFlags = 3;

	// Initialize everything
	InitMyChar();
	InitNpChar();
	InitBullet();
	InitCaret();
	InitStar();
	InitFade();
	InitFlash();
	ClearArmsData();
	ClearItemData();
	ClearPermitStage();
	StartMapping();
	InitFlags();
	InitBossLife();

	if (tCounter == 1)
	{
		if (!LoadProfile(*"Prof" + std::to_string(sCounter + 1).c_str()) && !InitializeGame())
			return 0;
	}
	else
	{
		if (!InitializeGame())
			return 0;
	}

	while (1)
	{
		// Get pressed keys
		GetTrg();

		if (gKey & KEY_PAUSE)
		{
			if (!saving) {
				switch (Call_Pause())
				{
					case enum_ESCRETURN_exit:
						return 0;

					case enum_ESCRETURN_restart:
						return 1;

					case enum_ESCRETURN_restart2:
						reloadN = true;
						return 1;
				}
			}
		}

		//DME2: Stage reloader
        if(gKeyTrg & KEY_R){
            PlaySoundObject(1, SOUND_MODE_PLAY);
            TransferStage2(gStageNo, 0, gMC.x, gMC.y);
			LoadGenericData();
        }

		if (g_GameFlags & 1)	// The "swPlay % 2" part is always true
		{
			if (g_GameFlags & 2)
				ActMyChar(TRUE);
			else
				ActMyChar(FALSE);
		}
		if (swPlay % gCurrentTimeScale + 1 && g_GameFlags & 1) {
			ActStar();
			ActNpChar();
			ActBossChar();
			ActValueView();
			ActBack();
			ResetMyCharFlag();
			HitMyCharMap();
			HitMyCharNpChar();
			HitMyCharBoss();
			HitNpCharMap();
			HitBossMap();
			HitBulletMap();
			HitNpCharBullet();
			HitBossBullet();
			if (g_GameFlags & 2)
				ShootBullet();
			ActBullet();
			ActCaret();
			MoveFrame3();
#ifdef FIX_MAJOR_BUGS
			// ActFlash uses frame_x and frame_y uninitialised
			GetFramePosition(&frame_x, &frame_y);
#endif
			ActFlash(frame_x, frame_y);
		}

		if (gCurrentTimeScale > 1)
			swPlay++;

		if (g_GameFlags & 1)	// The "swPlay % 2" part is always true
		{
			if (g_GameFlags & 2)
				AnimationMyChar(TRUE);
			else
				AnimationMyChar(FALSE);
		}

		if (g_GameFlags & 8)
		{
			ActionCredit();
			ActionIllust();
			ActionStripper();
		}

		ProcFade();
		CortBox(&grcFull, color);
		GetFramePosition(&frame_x, &frame_y);
		PutBack(frame_x, frame_y);
		PutStage_Back(frame_x, frame_y);
		PutBossChar(frame_x, frame_y);
		PutNpChar(frame_x, frame_y);
		PutBullet(frame_x, frame_y);
		PutMyChar(frame_x, frame_y);
		PutStar(frame_x, frame_y);
		PutMapDataVector(frame_x, frame_y);
		PutStage_Front(frame_x, frame_y);
		PutFront(frame_x, frame_y);
		PutBlackBars(frame_x, frame_y);
		PutFlash();
		PutCaret(frame_x, frame_y);
		PutValueView(frame_x, frame_y);
		PutBossLife();
		PutFade();

		if (!(g_GameFlags & 4))
		{
			// Open inventory
			if (gKeyTrg & gKeyItem)
			{
				BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);

				switch (CampLoop())
				{
					case enum_ESCRETURN_exit:
						return 0;

					case enum_ESCRETURN_restart:
						return 1;
				}

				gMC.cond &= ~1;
			}
			else if (gMC.equip & EQUIP_MAP && gKeyTrg & gKeyMap)
			{
				BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);

				switch (MiniMapLoop())
				{
					case enum_ESCRETURN_exit:
						return 0;

					case enum_ESCRETURN_restart:
						return 1;
				}
			}
		}

		if (g_GameFlags & 2)
		{
			if (gKeyTrg & gKeyArms)
				RotationArms();
			else if (gKeyTrg & gKeyArmsRev)
				RotationArmsRev();
		}

		if (swPlay % 2)	// This is always true
		{
			switch (TextScriptProc())
			{
				case enum_ESCRETURN_exit:
					return 0;

				case enum_ESCRETURN_restart:
					return 1;
			}
		}

		PutMapName(FALSE);
		PutTimeCounter(16, 8);

		if (g_GameFlags & 2)
		{
			PutMyLife(TRUE);
			PutArmsEnergy(TRUE);
			PutMyAir((WINDOW_WIDTH / 2) - 40, (WINDOW_HEIGHT / 2) - 16);
			PutActiveArmsList();
		}

		if (g_GameFlags & 8)
		{
			PutIllust();
			PutStripper();
		}

		PutTextScript();

		PutFramePerSecound();

		if (!Flip_SystemTask())
			return 0;

		++gCounter;
	}

	return 4;
}

BOOL Game(void)
{
	if (!LoadGenericData())
	{
	#if !defined(JAPANESE) && defined(FIX_BUGS) // The Aeon Genesis translation didn't translate this
		Backend_ShowMessageBox("Error", "Couldn't read general purpose files");
	#else
		Backend_ShowMessageBox("エラー", "汎用ファイルが読めない");
	#endif
		return FALSE;
	}

	PlaySoundObject(7, SOUND_MODE_PLAY_LOOP);

	std::string path = gDataPath + "/npc.tbl";

	if (!LoadNpcTable(path.c_str()))
	{
	#if !defined(JAPANESE) && defined(FIX_BUGS) // The Aeon Genesis translation didn't translate this
		Backend_ShowMessageBox("Error", "Couldn't read the NPC table");
	#else
		Backend_ShowMessageBox("エラー", "NPCテーブルが読めない");
	#endif
		return FALSE;
	}

	LoadStageTable();

	InitTextScript2();
	InitSkipFlags();
	InitMapData2();
	InitCreditScript();

	mode = 1;
	while (mode)
	{
		if (mode == 1)
			mode = ModeOpening();
		if (mode == 2)
			mode = ModeTitle();
		if (mode == 3)
			mode = ModeSaves();
		if (mode == 4)
			mode = ModeAction();
	}

	PlaySoundObject(7, SOUND_MODE_STOP);

	EndMapData();
	EndTextScript();
	ReleaseNpcTable();
	ReleaseCreditScript();

	return TRUE;
}
