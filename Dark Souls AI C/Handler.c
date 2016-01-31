#include "Source.h"
#include <stdlib.h>
#include "InitalizeFANN.h"
#include "Settings.h"

void BlackCrystalOut(){
    ResetVJoyController();
    //switch to black crystal
    iReport.bHats = ddown;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(1000); //gotta wait for menu to change
    //use
    iReport.lButtons = square;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    //yes i want to leave
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    iReport.lButtons = cross;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(500);
    //down d pad again to go back to red sign
    iReport.bHats = ddown;
    iReport.lButtons = 0x0;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(100);
    //wait
    iReport.bHats = dcenter;
    UpdateVJD(iInterface, (PVOID)&iReport);
    Sleep(10000);//10 sec is how long it takes to black crystal
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
static long LastRedSignTime = 0;

int main(void){
    #if FeedNeuralNet
        trainFromFile();
    #endif
    if (SetupandLoad()){
        return EXIT_FAILURE;
    }
    #if TrainNeuralNet
        SetupTraining();
    #endif

    while (1){
    #if AutoRedSign
        guiPrint(LocationHandler",0:RereadPointerEndAddress %d", RereadPointerEndAddress);
        guiPrint(LocationHandler",1:Enemy.loc_x %f\nvisualStatus %d", Enemy.loc_x, Player.visualStatus);
        guiPrint(LocationHandler",2:");

        if (RereadPointerEndAddress){
            ReadPointerEndAddresses(processHandle);
            ReadPlayer(&Enemy, processHandle, LocationMemoryEnemy);
            ReadPlayer(&Player, processHandle, LocationMemoryPlayer);
            ResetVJoyController();//just in case
            UpdateVJD(iInterface, (PVOID)&iReport);
        }
        ReadProcessMemory(processHandle, (LPCVOID)(Player.visualStatus_address), &(Player.visualStatus), 4, 0);//this memory read isnt directly AI related

        //if AI is a red phantom
        if (Player.visualStatus == 2){
            RedSignDown = false;
            //check that we got the enemy's struct address by ensuring their x loc is pos.
            if (Enemy.loc_x > 0){
                RereadPointerEndAddress = false;
            }
            //once one character dies
            if (Player.hp <= 0 || Enemy.hp <= 0){
                RereadPointerEndAddress = true;
            }

            //enemy player is fairly close
            if (distance(&Player, &Enemy) < 50){
            #if TrainNeuralNet
                GetTrainingData();
            #else
                MainLogicLoop();
            #endif
            }
            //if enemy player far away, black crystal out
            else if (!RereadPointerEndAddress){
                guiPrint(LocationHandler",2:BlackCrystalOut");
                RereadPointerEndAddress = true;
                BlackCrystalOut();
            }
        }
        //if AI in host world, and red sign not down, put down red sign
        else if (Player.visualStatus == 0){
            //ocasionally reput down red sign(failed to join session error catcher)
            if (!RedSignDown){
                guiPrint(LocationHandler",2:PutDownRedSign");
                Sleep(10000);//ensure we're out of loading screen
                PutDownRedSign();
            } else if (clock() >= LastRedSignTime + 360000){//6 min
                LastRedSignTime = clock();
                RedSignDown = false;
            }
        }
    #else
        #if TrainNeuralNet
            GetTrainingData();
        #else
            MainLogicLoop();
        #endif
    #endif
    }

    Exit();
}