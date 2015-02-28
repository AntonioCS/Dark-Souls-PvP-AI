#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

bool aboutToBeHit(Character * Player, Character * Phantom){
	return true;
}

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){

}

//get straight line distance between me and enemy
static double distance(Character * Player, Character * Phantom){
	double delta_x = abs(Player->loc_x - Phantom->loc_x);
	double delta_y = abs(Player->loc_y - Phantom->loc_y);
	return hypot(delta_x, delta_y);
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//if im farther away then my weapon can reach
	if (distance(Player, Phantom) > Player->weaponRange){
		//if we are not close enough, move towards 
		longTuple move = CoordsToJoystickAngle(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;

	}
}