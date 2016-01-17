#include "Source.h"
#include <stdlib.h>

void BlackCrystalOut(){
    ResetVJoyController();
    iReport.bHats = ddown;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(1000);//gotta wait for menu to change

    iReport.bHats = dcenter;
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);

    iReport.lButtons = cross;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);

    iReport.bHats = ddown;//down d pad again to go back to red sign
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);

    Sleep(100);
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(5000);//10 sec is how long it takes to black crystal
}

static bool RedSignDown = false;

void PutDownRedSign(){
    ResetVJoyController();
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    RedSignDown = true;
}

static bool RereadPointerEndAddress = true;

int main(void){
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }

    while (1){
        ReadProcessMemory(processHandle, (LPCVOID)(Player.visualStatus_address), &(Player.visualStatus), 4, 0);//this memory read isnt directly AI related

        if (RereadPointerEndAddress){
            ReadPointerEndAddresses(processHandle);
        }

        //if AI is a red phantom
        if (Player.visualStatus == 2){
            //enemy player is fairly close
            if(distance(&Player, &Enemy) < 50){
                MainLogicLoop();
                //once we die
                if (Player.hp <= 0){
                    RereadPointerEndAddress = true;
                }
            }
            //if enemy player far away, black crystal out
            else{
                RereadPointerEndAddress = true;
                BlackCrystalOut();
            }

            RedSignDown = false;
            //check that we got the enemy's struct address by ensuring their x loc is pos.
            if (Enemy.loc_x > 0){
                RereadPointerEndAddress = false;
            }
        }
        //if AI in host world, and red sign not down, put down red sign
        else if (Player.visualStatus == 0 && !RedSignDown){
            PutDownRedSign();
        }
    }

    Exit();
}