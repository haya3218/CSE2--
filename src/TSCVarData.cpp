//
// Created by Jake on 4/25/2021.
//

#include "Draw.h"
#include "TSCVarData.h"
#include "MyChar.h"

int varData[TSC_MAX];
int current_frame_x = 0, current_frame_y = 0;
int GetVariable(int id){
    switch (id){
        case 901:
            return gCurrentCion;
        case 902:
            return gMC.x;
        case 903:
            return gMC.y;
        case 904:
            return gMC.xm;
        case 905:
            return gMC.ym;
        default:
            return varData[id];
    }
}
void SetVariable(int id, int value){
    switch (id){
        case 901:
            gCurrentCion = value;
            break;

        case 902:
            gMC.x = value;
            break;
        case 903:
            gMC.y = value;
            break;
        case 904:
            gMC.xm = value;
            break;
        case 905:
            gMC.ym = value;
            break;

        default:
            varData[id] = value;
    }
}
void InitVarData(){
    for (int i = 0; i < TSC_MAX; ++i) {
        varData[i] = 0;
    }
}