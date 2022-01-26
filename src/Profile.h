// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#pragma once

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "MyChar.h"
#include "SelStage.h"
#include "Stage.h"

typedef struct PROFILEDATA
{
	char code[8];
	int stage = -1;
	MusicID music;
	int x;
	int y;
	int direct;
	short max_life;
	short star;
	short life;
	short a;
	int select_arms;
	int select_item;
	int equip;
	int unit;
	int counter;
	ARMS arms[8];
	ITEM items[32];
	PERMIT_STAGE permitstage[8];
	signed char permit_mapping[0x80];
	char FLAG[4];
	unsigned char flags[1000];

	// Custom
	char extra_code[0x10];
	// <MIM
	unsigned int MIMCurrentNum;
	// <PHY
	MYCHAR_PHYSICS physics_normal;
	MYCHAR_PHYSICS physics_underwater;
	int no_splash_or_air_limit_underwater;
	// equip+ shit
	int health_threshold = 0;
	int alt_health_threshold = 0;
	int health_regen = 0;
	int health_regen_am = 0;
	int alt_health_regen = 0;
	int alt_health_regen_am = 0;
} PROFILEDATA;

extern const char* gDefaultName;
extern const char* const gProfileCode;

BOOL IsProfile(void);
BOOL IsProfileNum(void);
BOOL SaveProfile(const char *name);
BOOL LoadProfile(const char *name);
PROFILEDATA LoadProfileSimple(int num);
BOOL InitializeGame(void);
