#include "AIMethods.h"

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to long

//get straight line distance between me and enemy
static double distance(Character * Player, Character * Phantom){
	double delta_x = fabsf(fabsf(Player->loc_x) - fabsf(Phantom->loc_x));
	double delta_y = fabsf(fabsf(Player->loc_y) - fabsf(Phantom->loc_y));
	return hypot(delta_x, delta_y);
}

/*determine what type of action the animation id is.

i cant seem to find a pattern in the ids, so this is just a big switch statement

ranged attacks use a different format: they have a specific animation for windup,recover, and hurtbox creation; while others rely on a subanimation id to determine windup, hurtbox, and recovery.
because of this, have to specify if we need to look at subanimation
0 is not attack animation, 1 is windup to attack, 2 is attack id but must check subanimation(hurtbox not instantly generated), 3 is hurtbox is created*/
static unsigned char isAttackAnimation(unsigned char animation_id){
	switch (animation_id){
	//nothing
	case 0:
		return 0;
	//could not use
	case 21:
		return 0;
	//1 hand roll
	case 32:
		return 0;
	//backstep 1h
	case 38:
		return 0;
	//1h rolling attack
	case 41:
		return 2;
	//1h r1
	case 46:
		return 2;
	//1h r1 swing 2
	case 48:
		return 2;
	//1h r1 swing 3
	case 49:
		return 2;
	//jumping 1 hand
	case 53:
		return 2;
	//left hand attack
	case 55:
		return 2;
	//backstep attack 1h
	case 59:
		return 2;
	//backstep attack 2h
	case 60:
		return 2;
	//crossbow/bow windup 2h
	case 65:
		return 0;
	//bow hold
	case 66:
		return 0;
	//crossbow/bow attack 2h
	case 67:
		return 3;
	//crossbow recover 2h
	case 68:
		return 0;
	//crossbow windup 1h
	case 69:
		return 0;
	//crossbow attack 1h
	case 70:
		return 3;
	//crossbow recover 1h
	case 71:
		return 0;
	//shield raise
	case 76:
		return 0;
	//shield up
	case 77:
		return 0;
	//shied lower
	case 78:
		return 0;
	//parry
	case 86:
		return 1;//not a windup, but pretend it is to prevent attack
	//1h r2
	case 89:
		return 2;
	//rolling attack 2h
	case 103:
		return 2;
	//2h r1
	case 107:
		return 2;
	//2h r1 bounce back
	case 108:
		return 0;
	//2h r1 combo
	case 109:
		return 2;
	//2h r1 combo
	case 110:
		return 2;
	//jumping 2 hand
	case 113:
		return 2;
	//2h r2
	case 115:
		return 2;
	//firestorm windup
	case 163:
		return 0;//is a windup, but want to attack during it
	//firstorm
	case 164:
		return 2;
	//flame windup
	case 167:
		return 1;
	//flame attack
	case 168:
		return 2;
	//pyro ball windup
	case 173:
		return 1;
	//pyro ball throw
	case 174:
		return 2;
	//backstep 2h
	case 100:
		return 0;
	//1 hand weapon
	case 185:
		return 0;
	//1 hand weapon
	case 188:
		return 0;
	//2 hand weapon
	case 199:
		return 0;
	//2 hand weapon
	case 203:
		return 0;
	//weapon switch r
	case 209:
		return 0;
	//weapon switch r
	case 213:
		return 0;
	//weapon switch l
	case 217:
		return 0;
	//weapon switch l
	case 221:
		return 0;
	//backstab
	case 225:
		return 0;
	default:
		printf("unknown animation id\n");
		return 0;
	}
}

unsigned char aboutToBeHit(Character * Player, Character * Phantom){
	//if they are outside of their attack range, we dont have to do anymore checks
	if (distance(Player, Phantom) <= Phantom->weaponRange){
		unsigned char AtkID = isAttackAnimation(Phantom->animation_id);
		//attack id will tell up if an attack is coming up soon. if so, we need to prevent going into a subroutine(attack), and wait for attack to fully start b4 entering dodge subroutine

		//TODO dodging too mutch, need to find when enemy hurtbox stops
		//also dodging too late(combustion) or too early(sword)
		if (
			//if enemy is in attack animation, 
			AtkID>1
			//this is the range attack edge case or attack animation about to generate hurtbox(check sub animation)
			//TODO if i can know how far in the windup we are, i can utalize time in windup before hurtbox and still dodge in time
			&& ((AtkID == 3) || (Phantom->weightanimation) == 1.0)
			//and their attack will hit me(their rotation is correct and their weapon hitbox width is greater than their rotation delta)
			//&& (Phantom->rotation)>((Player->rotation) - 3.1) && (Phantom->rotation)<((Player->rotation) + 3.1)
			){
			printf("about to be hit ");
			return 2;
		}
		//windup, attack coming
		else if (AtkID){
			printf("dont attack\n");
			return 1;
		}
	}

	printf("not about to be hit\n");
	return 0;
}

#define inputDelayForDodge 30
#define inputDelayForStopDodge 50

//initiate the dodge command logic. This can be either toggle escaping, rolling, or parrying.
void dodge(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//procede with subroutine if we are not in one already
	if (!inActiveSubroutine()){
		//indicate we are in dodge subroutine
		subroutine_states[0] = 1;
		//set time for this subroutine
		startTime = clock();
	}

	if (subroutine_states[0]){
		//dodge at a 5 degree angle
		double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
		angle += 5;
		//angle joystick
		longTuple move = angleToJoystick(angle);
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;

		//after the joystick input, press circle to roll but dont hold circle, otherwise we run
		long curTime = clock();
		if ((curTime >= startTime + inputDelayForDodge) && (curTime < startTime + inputDelayForStopDodge)){
			iReport->lButtons = circle;
		}
		
		//end subanimation on recover animation
		if (Player->subanimation == AttackSubanimationRecover){
			printf(" end sub ");
			subroutine_states[0] = 0;
		}
		printf("dodge\n");
	}
}

#define inputDelayForKick 30
#define inputDelayForRotateBack 60

static void ghostHit(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//procede with subroutine if we are not in one already
	if (!inActiveSubroutine()){
		//indicate we are in attack subroutine
		subroutine_states[1] = 1;
		//set time for this subroutine
		startTime = clock();
	}

	if (subroutine_states[1]){
		//always hold attack button
		iReport->lButtons = r1;

		//cant angle joystick immediatly, at first couple frames this will register as kick
		//wait time, then trigger next stage of routine
		//printf(" %d ", (long)clock());
		if (clock() >= startTime + inputDelayForKick){
			subroutine_states[1] = 2;
		}
		//start rotate back(Player->subanimation == 65792 marks point where we can NO longer turn back, CANT use that as flag, need to track internally)
		if (clock() >= startTime + inputDelayForRotateBack){
			subroutine_states[1] = 3;
		}

		//point away from enemy till end of windup
		if (Player->subanimation == AttackSubanimationWindup && subroutine_states[1] == 2){
			printf("away");
			double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
			angle = fabs(angle - 180.0);//TODO this doesnt work for some angles
			longTuple move = angleToJoystick(angle);
			iReport->wAxisX = move.first;
			iReport->wAxisY = move.second;
		}

		//point towards enemy during active part of animation
		else if (Player->subanimation == AttackSubanimationWindup && subroutine_states[1] == 3){
			printf("towards");
			double angle = angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y);
			longTuple move = angleToJoystick(angle);
			iReport->wAxisX = move.first;
			iReport->wAxisY = move.second;
		}

		//end subanimation on recover animation
		else if (Player->subanimation == AttackSubanimationRecover){
			subroutine_states[1] = 0;
			//release attack button and joystick
			iReport->lButtons = 0x000000000;
			iReport->wAxisX = MIDDLE;
			iReport->wAxisY = MIDDLE;
			printf("end sub");
		}
		printf("\n");
	}
}

//initiate the attack command logic. This can be a standard(physical) attack or a backstab.
void attack(Character * Player, Character * Phantom, JOYSTICK_POSITION * iReport){
	//TODO need timing analysis. Opponent can move outside range during windup

	//if im farther away then my weapon can reach, and we're not in a subroutine
	if (!inActiveSubroutine() && distance(Player, Phantom) > Player->weaponRange){
		printf("move to attack\n");
		//if we are not close enough, move towards 
		longTuple move = angleToJoystick(angleFromCoordinates(Player->loc_x, Phantom->loc_x, Player->loc_y, Phantom->loc_y));
		iReport->wAxisX = move.first;
		iReport->wAxisY = move.second;
	}else{
		//TODO will need to differentiate different attack subroutines

		//otherwise, decide between attack and backstab
		printf("attack ");

		//(always use ghost hits for normal attacks)
		//ghostHit(Player, Phantom, iReport);
	}
}