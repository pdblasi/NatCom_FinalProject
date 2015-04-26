#include <forward_list>

using namespace std;

#ifndef __PLAYER_H__
#define __PLAYER_H__

struct position
{
    int x;
    int y;

    position(int _x, int _y): x(_x), y(_y) { };
    position(const position &orig): x(orig.x), y(orig.y) { };
};

struct player
{
    forward_list<position> critter_positions;
    forward_list<int> pop_changes;

    int herd_mentality;
    int prey_mentality;
};

#endif
