// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include <cstdlib>
#include "Shoot.h"

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "Bullet.h"
#include "Draw.h"
#include "Caret.h"
#include "CommonDefines.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "MycParam.h"
#include "Sound.h"
#include "Game.h"

static int empty;
int shoot_timeout = 0;

void ShootBullet_Frontia1(int level)
{
	int bul_no;

    if (level == 1)
        bul_no = 1;
    else if (level == 2)
        bul_no = 2;
    else
        bul_no = 3;

	if (CountArmsBullet(1) > 3)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			ChangeToFirstArms();
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y - (10 * 0x200), 1);
					SetCaret(gMC.x - (3 * 0x200), gMC.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y - (10 * 0x200), 1);
					SetCaret(gMC.x + (3 * 0x200), gMC.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y + (10 * 0x200), 3);
					SetCaret(gMC.x - (3 * 0x200), gMC.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y + (10 * 0x200), 3);
					SetCaret(gMC.x + (3 * 0x200), gMC.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (2 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (2 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}

			PlaySoundObject(33, SOUND_MODE_PLAY);
		}
	}
}
int poleRan = 1;
void ShootBullet_PoleStar(int level)
{
    int firerate;
	int bul_no;
	int lifetime = 20+level*2;
	if(lifetime > 600){
	    lifetime = 600;
	}
    if(level <= 1){
        bul_no = 4;
    }else if(level <= 2){
        bul_no = 5;
    }else{
        bul_no = 6;
    }
    /// fire rate
    firerate = 10-(level/7);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    //gKeyTrg & gKeyShot
    int key = (level < 10) ? gKeyTrg : gKey;
	if (key & gKeyShot)
	{
	    //Shoot timeout stuff
	    if(shoot_timeout > 0 && level > 9){
            return;
	    }else{
	        shoot_timeout = firerate;
	    }
	    //
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, SOUND_MODE_PLAY);
		}
		else
		{
		    int spawn_x = 0;
		    int spawn_y = 0;
		    int spawn_xm = 0;
		    int spawn_ym = 0;
		    int dir = DIR_LEFT;
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
				    dir = DIR_UP;
				    spawn_x = gMC.x - (1 * 0x200);
				    spawn_y = gMC.y - (8 * 0x200);
					spawn_ym = 0x200;
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}

			float spawn_vel = (level*0.5f);
			if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 8) spawn_vel = 8;
			spawn_vel *= -1;
			spawn_xm *= (spawn_vel);
			spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/8;
            int offset_power = 0x200*(level*0.08);
            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
            }else{
                for (int i = 0; i < bul_count; ++i) {
                    int rx = (offset_power/2)-(rand()%offset_power);
                    int ry = (offset_power/2)-(rand()%offset_power);
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                }
            }


			if (level == 3)
				PlaySoundObject(49, SOUND_MODE_PLAY);
			else
				PlaySoundObject(32, SOUND_MODE_PLAY);
		}
	}
}

void ShootBullet_FireBall(int level)
{
    int firerate;
    int bul_no;
    int lifetime = 500;
    if(level <= 1){
        bul_no = 7;
    }else if(level <= 2){
        bul_no = 8;
    }else{
        bul_no = 9;
    }
    /// fire rate
    firerate = 10-(level/7);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    //gKeyTrg & gKeyShot
    int key = (level < 10) ? gKeyTrg : gKey;
    if (key & gKeyShot && (poleRan++%firerate==0))
    {
        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x100;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x100;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x100;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x100;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x100;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x100;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.01f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 8) spawn_vel = 8;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/30;
            int offset_power = 0x100*(level*0.08);
            int rx = rand()%offset_power - offset_power/2;
            int ry = rand()%offset_power - offset_power/2;
//            for (int i = 0; i < bul_count; ++i) {
                SetBulletExt(bul_no, spawn_x + rx, spawn_y + ry, spawn_xm*3 + rx, spawn_ym*3 + ry, lifetime, dir);
//            }


            if (level == 3)
                PlaySoundObject(49, SOUND_MODE_PLAY);
            else
                PlaySoundObject(32, SOUND_MODE_PLAY);
        }
    }
}

void ShootBullet_Machinegun1(int level)
{
	///
    int firerate;
    int bul_no;
    int lifetime = 20+level*5;
    if(level <= 1){
        bul_no = 10;
    }else if(level <= 2){
        bul_no = 11;
    }else{
        bul_no = 12;
    }
    /// fire rate
    firerate = 10-(level/4);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    //gKeyTrg & gKeyShot
    if (gKey & gKeyShot && (poleRan++%firerate==0))
    {
        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.07f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 8) spawn_vel = 8;
            float gMC_mult = 0.3f *(level/10.0);
            gMC.xm += spawn_xm*gMC_mult;
            gMC.ym += spawn_ym*gMC_mult;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
//            int bul_count = 1+(level)/8;
//            int offset_power = 0x200*(level*0.08);
//            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
//            }else{
//                for (int i = 0; i < bul_count; ++i) {
//                    int rx = (offset_power/2)-(rand()%offset_power);
//                    int ry = (offset_power/2)-(rand()%offset_power);
//                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
//                }
//            }


            PlaySoundObject(49, SOUND_MODE_PLAY);
        }
    }
}

void ShootBullet_Missile(int level, BOOL bSuper)
{
    //28, 29, 30
    int firerate;
    int bul_no;
    int lifetime = 20+level*5;
    if(level <= 1){
        bul_no = 28;
    }else if(level <= 2){
        bul_no = 29;
    }else{
        bul_no = 30;
    }
    /// fire rate
    firerate = 10-(level/7);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    int key = (level < 50) ? gKeyTrg : gKey;
    if (key & gKeyShot)
    {
        //Shoot timeout stuff
        if(shoot_timeout > 0){
            return;
        }else{
            shoot_timeout = firerate;
        }
        //

        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.07f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 50) spawn_vel = 50;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/8;
            int offset_power = 0x200*(level*0.08);
            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
            }else{
                for (int i = 0; i < bul_count; ++i) {
                    int rx = (offset_power/2)-(rand()%offset_power);
                    int ry = (offset_power/2)-(rand()%offset_power);
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                }
            }


            if (level == 3)
                PlaySoundObject(49, SOUND_MODE_PLAY);
            else
                PlaySoundObject(32, SOUND_MODE_PLAY);
        }
    }
}

void ShootBullet_Explos(int level, BOOL bSuper)
{
    //28, 29, 30
    int firerate;
    int bul_no = 47;
    int lifetime = 20+level*5;
    /// fire rate
    firerate = 10-(level/7);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    int key = (level < 50) ? gKeyTrg : gKey;
    if (key & gKeyShot)
    {
        //Shoot timeout stuff
        if(shoot_timeout > 0){
            return;
        }else{
            shoot_timeout = firerate;
        }
        //

        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.07f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 50) spawn_vel = 50;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/8;
            int offset_power = 0x200*(level*0.08);
            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
            }else{
                for (int i = 0; i < bul_count; ++i) {
                    int rx = (offset_power/2)-(rand()%offset_power);
                    int ry = (offset_power/2)-(rand()%offset_power);
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                }
            }


            if (level == 3)
                PlaySoundObject(49, SOUND_MODE_PLAY);
            else
                PlaySoundObject(32, SOUND_MODE_PLAY);
        }
    }
}

void ShootBullet_Bubblin1_OLD()
{
	static int wait;

	if (CountArmsBullet(7) > 3)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, SOUND_MODE_PLAY);

			if (empty == 0)
			{
				SetCaret(gMC.x, gMC.y, CARET_EMPTY, DIR_LEFT);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (1 * 0x200), gMC.y - (2 * 0x200), 1);
				SetCaret(gMC.x - (1 * 0x200), gMC.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(19, gMC.x + (1 * 0x200), gMC.y - (2 * 0x200), 1);
				SetCaret(gMC.x + (1 * 0x200), gMC.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}
		else if (gMC.down)
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (1 * 0x200), gMC.y + (2 * 0x200), 3);
				SetCaret(gMC.x - (1 * 0x200), gMC.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(19, gMC.x + (1 * 0x200), gMC.y + (2 * 0x200), 3);
				SetCaret(gMC.x + (1 * 0x200), gMC.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}
		else
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
				SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(19, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
				SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}

		PlaySoundObject(48, SOUND_MODE_PLAY);
	}
	else if (++wait > 20)
	{
		wait = 0;
		ChargeArmsEnergy(1);
	}
}

void ShootBullet_Bubblin2(int level)
{
	static int wait;

	if (CountArmsBullet(7) > 15)
		return;

	level += 18;

	if (!(gKey & gKeyShot))
		gMC.rensha = 6;

	if (gKey & gKeyShot)
	{
		if (++gMC.rensha < 7)
			return;

		gMC.rensha = 0;

		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, SOUND_MODE_PLAY);

			if (empty == 0)
			{
				SetCaret(gMC.x, gMC.y, CARET_EMPTY, DIR_LEFT);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x - (3 * 0x200), gMC.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(level, gMC.x + (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x + (3 * 0x200), gMC.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}
		else if (gMC.down)
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x - (3 * 0x200), gMC.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(level, gMC.x + (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x + (3 * 0x200), gMC.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}
		else
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
				SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
			else
			{
				SetBullet(level, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
				SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
			}
		}

		PlaySoundObject(48, SOUND_MODE_PLAY);
	}
	else if (++wait > 1)
	{
		wait = 0;
		ChargeArmsEnergy(1);
	}
}

void ShootBullet_Sword(int level)
{
    //25,56,57
    int firerate;
    int bul_no;
    int lifetime = 20+level*5;
    if(level <= 1){
        bul_no = 25;
    }else if(level <= 2){
        bul_no = 26;
    }else{
        bul_no = 27;
    }
    /// fire rate
    firerate = 10-(level/7);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    int key = (level < 20) ? gKeyTrg : gKey;
    if (key & gKeyShot)
    {
        //Shoot timeout stuff
        if(shoot_timeout > 0){
            return;
        }else{
            shoot_timeout = firerate;
        }
        //
        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.07f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 8) spawn_vel = 8;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/8;
            int offset_power = 0x200*(level*0.08);
            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
            }else{
                for (int i = 0; i < bul_count; ++i) {
                    int rx = (offset_power/2)-(rand()%offset_power);
                    int ry = (offset_power/2)-(rand()%offset_power);
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                }
            }


            if (level == 3)
                PlaySoundObject(49, SOUND_MODE_PLAY);
            else
                PlaySoundObject(32, SOUND_MODE_PLAY);
        }
    }
}

void ShootBullet_Nemesis(int level)
{
    int firerate;
    int bul_no;
    int lifetime = 20+level*5;
    if(level <= 1){
        bul_no = 36;
    }else if(level <= 5){
        bul_no = 35;
    }else{
        bul_no = 34;
    }
    /// fire rate
    firerate = 10-(level/4);
    if(firerate < 1) firerate = 1;

//	if (CountArmsBullet(2) > 1)
//		return;
    int key = (level < 10) ? gKeyTrg : gKey;
    if (key & gKeyShot)
    {
        //Shoot timeout stuff
        if(shoot_timeout > 0){
            return;
        }else{
            shoot_timeout = firerate;
        }
        //
        if (!UseArmsEnergy(1))
        {
            PlaySoundObject(37, SOUND_MODE_PLAY);
        }
        else
        {
            int spawn_x = 0;
            int spawn_y = 0;
            int spawn_xm = 0;
            int spawn_ym = 0;
            int dir = DIR_LEFT;
            if (gMC.up)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_UP;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y - (8 * 0x200);
                    spawn_ym = 0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else if (gMC.down)
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x - (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_DOWN;
                    spawn_x = gMC.x + (1 * 0x200);
                    spawn_y = gMC.y + (8 * 0x200);
                    spawn_ym = -0x200;
                    SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }
            else
            {
                if (gMC.direct == 0)
                {
                    dir = DIR_LEFT;
                    spawn_x = gMC.x - (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = 0x200;
                    SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
                else
                {
                    dir = DIR_RIGHT;
                    spawn_x = gMC.x + (6 * 0x200);
                    spawn_y = gMC.y + (3 * 0x200);
                    spawn_xm = -0x200;
                    SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                }
            }

            float spawn_vel = (level*0.07f);
            if(spawn_vel < 1) spawn_vel = 1;
            if(spawn_vel > 8) spawn_vel = 8;
            spawn_vel *= -1;
            spawn_xm *= (spawn_vel);
            spawn_ym *= (spawn_vel);
            int bul_count = 1+(level)/8;
            int offset_power = 0x200*(level*0.08);
            if(level < 30){
                SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
            }else{
                for (int i = 0; i < bul_count; ++i) {
                    int rx = (offset_power/2)-(rand()%offset_power);
                    int ry = (offset_power/2)-(rand()%offset_power);
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                }
            }


            if (level == 1)
                PlaySoundObject(60, SOUND_MODE_PLAY);
            else
                PlaySoundObject(117, SOUND_MODE_PLAY);
        }
    }
}

int spur_charge;

void ResetSpurCharge(void)
{
	spur_charge = 0;

//	if (gArmsData[gSelectedArms].code == 13)
//		ZeroExpMyChar();
}

void ShootBullet_Spur(int level)
{
    if(true){
        int firerate;
        int bul_no;
        int lifetime = 30+level*2;
        if(lifetime > 100){
            lifetime = 100;
        }
        if(level <= 1){
            bul_no = 37;
        }else if(level <= 2){
            bul_no = 38;
        }else{
            bul_no = 39;
        }
        /// fire rate
        firerate = 25-(level/5);
        if(firerate < 1) firerate = 1;

        if (gKey & gKeyShot)
        {
            //Shoot timeout stuff
            if(shoot_timeout > 0){
                return;
            }else{
                shoot_timeout = firerate;
            }
            //
            if (!UseArmsEnergy(1))
            {
                PlaySoundObject(37, SOUND_MODE_PLAY);
            }
            else
            {
                int spawn_x = 0;
                int spawn_y = 0;
                int spawn_xm = 0;
                int spawn_ym = 0;
                int dir = DIR_LEFT;
                if (gMC.up)
                {
                    if (gMC.direct == 0)
                    {
                        dir = DIR_UP;
                        spawn_x = gMC.x - (1 * 0x200);
                        spawn_y = gMC.y - (8 * 0x200);
                        spawn_ym = 0x200;
                        SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                    else
                    {
                        dir = DIR_UP;
                        spawn_x = gMC.x + (1 * 0x200);
                        spawn_y = gMC.y - (8 * 0x200);
                        spawn_ym = 0x200;
                        SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                }
                else if (gMC.down)
                {
                    if (gMC.direct == 0)
                    {
                        dir = DIR_DOWN;
                        spawn_x = gMC.x - (1 * 0x200);
                        spawn_y = gMC.y + (8 * 0x200);
                        spawn_ym = -0x200;
                        SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                    else
                    {
                        dir = DIR_DOWN;
                        spawn_x = gMC.x + (1 * 0x200);
                        spawn_y = gMC.y + (8 * 0x200);
                        spawn_ym = -0x200;
                        SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                }
                else
                {
                    if (gMC.direct == 0)
                    {
                        dir = DIR_LEFT;
                        spawn_x = gMC.x - (6 * 0x200);
                        spawn_y = gMC.y + (3 * 0x200);
                        spawn_xm = 0x200;
                        SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                    else
                    {
                        dir = DIR_RIGHT;
                        spawn_x = gMC.x + (6 * 0x200);
                        spawn_y = gMC.y + (3 * 0x200);
                        spawn_xm = -0x200;
                        SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
                    }
                }

                float spawn_vel = (level*0.05f);
                if(spawn_vel < 1) spawn_vel = 1;
                if(spawn_vel > 2.5) spawn_vel = 2.5;
                spawn_vel *= -1;
                spawn_xm *= (spawn_vel);
                spawn_ym *= (spawn_vel);
                int bul_count = 1+(level)/40;
                int offset_power = 0x200*(level*0.01);
                if(level < 160){
                    SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3, spawn_ym*3, lifetime, dir);
                }else{
                    for (int i = 0; i < bul_count; ++i) {
                        int rx = (offset_power/2)-(rand()%offset_power);
                        int ry = (offset_power/2)-(rand()%offset_power);
                        SetBulletExt(bul_no, spawn_x, spawn_y, spawn_xm*3+rx, spawn_ym*3+ry, lifetime, dir);
                    }
                }


                if(level <= 10) {
                    PlaySoundObject(62, SOUND_MODE_PLAY);
                }else if(level <= 20) {
                    PlaySoundObject(63, SOUND_MODE_PLAY);
                }else{
                    PlaySoundObject(64, SOUND_MODE_PLAY);
                }
            }
        }
        return;
    }
	int bul_no;
	BOOL bShot;
	static BOOL bMax;

	bShot = TRUE;

//	if (gKey & gKeyShot)
//	{
//		if (gMC.equip & EQUIP_TURBOCHARGE)
//			AddExpMyChar(3);
//		else
//			AddExpMyChar(2);
//
//		if (++spur_charge / 2 % 2)
//		{
//			switch (level)
//			{
//				case 1:
//					PlaySoundObject(59, SOUND_MODE_PLAY);
//					break;
//
//				case 2:
//					PlaySoundObject(60, SOUND_MODE_PLAY);
//					break;
//
//				case 3:
//					if (!IsMaxExpMyChar())
//						PlaySoundObject(61, SOUND_MODE_PLAY);
//
//					break;
//			}
//		}
//	}
//	else
//	{
//		if (spur_charge)
//			bShot = TRUE;
//
//		spur_charge = 0;
//	}

//	if (IsMaxExpMyChar())
//	{
//		if (!bMax)
//		{
//			bMax = TRUE;
//			PlaySoundObject(65, SOUND_MODE_PLAY);
//		}
//	}
//	else
//	{
//		bMax = FALSE;
//	}

//	if (!(gKey & gKeyShot))
//		ZeroExpMyChar();

	if(level < 1){
	    bul_no = 37;
	}else if(level < 2){
	    bul_no = 38;
	}else{
        bul_no = 39;
	}


	if (gKey & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, SOUND_MODE_PLAY);
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
                        SetBulletExt(bul_no, gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 0x100,0x400, 100, 1);
                        SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
				}
			}

			switch (bul_no)
			{
				case 37:
					PlaySoundObject(62, SOUND_MODE_PLAY);
					break;

				case 38:
					PlaySoundObject(63, SOUND_MODE_PLAY);
					break;

				case 39:
					PlaySoundObject(64, SOUND_MODE_PLAY);
					break;
			}
		}
	}
}
BOOL min_active = TRUE;
int min(int a, int b){
    if(min_active){
        if(a < b) return a;
        else return b;
    }else{
        if(a > b) return a;
        else return b;
    }
}
int everran = 0;
void ShootBullet_Everything(int level) {
    int bullets = 1;
    if (level > 500) {
        bullets = 2;
    } else if (level > 1000) {
        bullets = 3;
    }
    if(level > 3000){
        min_active = FALSE;
    }
    for (int i = 0; i < bullets; ++i) {
        int spur_level = min(level/3, 1000);
        int missile_level = min(level, 500);
        int blade_level = min(level/3, 300);
        int polarstar_level = min(level/2, 300);
        int fireball_level = min(level/4, 100);
        int nemisis_level = min(level/2, 900);

        switch (everran++ % 6) {
            case 0:
                ShootBullet_Spur(spur_level);
                break;
            case 1:
                ShootBullet_Missile(missile_level, TRUE);
                break;
            case 2:
                ShootBullet_Sword(blade_level);
                break;
            case 3:
                ShootBullet_PoleStar(polarstar_level);
                break;
            case 4:
                ShootBullet_FireBall(fireball_level + 1);
                break;
            case 5:
                ShootBullet_Nemesis(nemisis_level);
                break;
        }
    }
}
void ShootBullet(void)
{
    shoot_timeout--;
    if(shoot_timeout < 0) shoot_timeout = 0;
	static int soft_rensha;	// 'rensha' is Japanese for 'rapid-fire', apparently

	if (empty != 0)
		--empty;

	// Only let the player shoot every 4 frames
	if (soft_rensha != 0)
		--soft_rensha;

//	if (gKeyTrg & gKeyShot)
//	{
//		if (soft_rensha != 0)
//			return;
//
//		soft_rensha = 4;
//	}

	// Run functions
	if (gMC.cond & 2)
		return;

	switch (gArmsData[gSelectedArms].code)
	{
		case 1:
			ShootBullet_Frontia1(gArmsData[gSelectedArms].level);
			break;

		case 2:
			ShootBullet_PoleStar(gArmsData[gSelectedArms].level);
			break;

		case 3:
			ShootBullet_FireBall(gArmsData[gSelectedArms].level + 1);
			break;

		case 4:
			ShootBullet_Machinegun1(gArmsData[gSelectedArms].level);
			break;

		case 5:
            ShootBullet_Explos(gArmsData[gSelectedArms].level, TRUE);
			//ShootBullet_Missile(gArmsData[gSelectedArms].level, TRUE);
			break;

        case 6:
			ShootBullet_Explos(gArmsData[gSelectedArms].level, TRUE);
			break;

		case 7:
            ShootBullet_Explos(gArmsData[gSelectedArms].level, TRUE);
            ShootBullet_Everything(gArmsData[gSelectedArms].level);
//			switch (gArmsData[gSelectedArms].level)
//			{
//				case 1:
//					ShootBullet_Bubblin1();
//					break;
//
//				case 2:
//					ShootBullet_Bubblin2(2);
//					break;
//
//				case 3:
//					ShootBullet_Bubblin2(3);
//					break;
//			}

			break;

		case 9:
            ShootBullet_Sword(gArmsData[gSelectedArms].level);

			break;

		case 10:
			ShootBullet_Missile(gArmsData[gSelectedArms].level, TRUE);
			break;

		case 12:
			ShootBullet_Nemesis(gArmsData[gSelectedArms].level);
			break;

		case 13:
			ShootBullet_Spur(gArmsData[gSelectedArms].level);
			break;
	}
}
