#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <forward_list>
#include "Globals.h"

using namespace std;

struct position
{
    int x;
    int y;

    position(int _x, int _y): x(_x), y(_y) { }
    position(const position &orig): x(orig.x), y(orig.y) { }

    bool operator==(const position &other)
    {
        return (this->x == other.x) && (this->y == other.y);
    }
};

struct player
{
    forward_list<position> critter_positions;
    int pop_change;
    int tot_pop = 0;
    float spawn_x;
    float spawn_y;

    float herd_mentality; // -5 to +5
    float prey_mentality; // -5 to +5
    float vision; // 1 to 5
    float flight_mentality; // -5 to +5
    float lifespan; // -5 to +5
};

#endif
