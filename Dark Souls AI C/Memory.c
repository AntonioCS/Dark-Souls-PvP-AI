#include "Memory.h"

unsigned char last_subroutine_states_self[last_subroutine_states_self_LENGTH];

void AppendLastSubroutineSelf(unsigned char subroutineId){
    for (unsigned int i = last_subroutine_states_self_LENGTH - 1; i > 1; i--){
        last_subroutine_states_self[i] = last_subroutine_states_self[i - 1];
    }
    last_subroutine_states_self[0] = subroutineId;
}

int last_animation_ids_enemy[last_animation_ids_enemy_LENGTH];

//handles check that the new aid to add isnt the same as the most recent old one. This can't happen from attacks, because -1 it always between two attacks fo the same aid.
bool AppendLastAnimationIdEnemy(int aid){
    if (aid != last_animation_ids_enemy[0]){
        for (unsigned int i = last_animation_ids_enemy_LENGTH - 1; i > 1; i--){
            last_animation_ids_enemy[i] = last_animation_ids_enemy[i - 1];
        }
        last_animation_ids_enemy[0] = aid;
        return true;
    }
    return false;
}

unsigned short last_animation_types_enemy[last_animation_types_enemy_LENGTH];//update every 100 ms. LENGTH*100 = time of memory

static long lastAnimationTypeUpdateTime = 0;

void AppendAnimationTypeEnemy(unsigned short animationType_id){
    if (clock() - lastAnimationTypeUpdateTime >= 100){
        for (unsigned int i = last_animation_types_enemy_LENGTH - 1; i > 1; i--){
            last_animation_types_enemy[i] = last_animation_types_enemy[i - 1];
        }
        last_animation_types_enemy[0] = animationType_id;

        lastAnimationTypeUpdateTime = clock();
    }
}

float DistanceMemory[DistanceMemoryLENGTH];//update every 100 ms.

static long LastDistanceUpdateTime = 0;

void AppendDistance(float distance){
    if (clock() - LastDistanceUpdateTime >= 100){
        for (unsigned int i = DistanceMemoryLENGTH - 1; i > 1; i--){
            DistanceMemory[i] = DistanceMemory[i - 1];
        }
        DistanceMemory[0] = distance;

        LastDistanceUpdateTime = clock();
    }
}