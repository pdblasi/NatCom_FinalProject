#include "globals.h"
#include "player.h"
#include <cstdlib>
#include <ctime>

using namespace std;

#ifndef __SERVER_H__
#define __SERVER_H__

forward_list<player> PLAYERS;

void DEBUG_SETUP();

// Use this to modify the NEXT maps
void generateNextStep();
void decayPheromones();
void moveCritters();
void moveCritter(position &p, int herd, int pred, int playerNum);
void handleConflicts();

// Use this to set up the VALID_MAP bool array
// Also initializes other maps
void generateMap();
// Use this to copy the NEXT maps to their counterparts.
void stepMapsForward();

// Dealloc all resources
void exterminate();

struct square
{
    int x, y;
    int odds;

    square(int _x, int _y): x(_x), y(_y) { };
};

#endif
