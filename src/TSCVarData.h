//
// Created by Jake on 4/25/2021.
//

#pragma once

#include "WindowsWrapper.h"

#define TSC_MAX 9999

int GetVariable(int id);
void SetVariable(int id, int value);
void InitVarData();
extern int current_frame_x, current_frame_y;
