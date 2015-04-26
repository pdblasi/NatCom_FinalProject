#include <forward_list>

using namespace std;

#ifndef __PLAYER_H__
#define __PLAYER_H__

struct position
{
    int x;
    int y;
};

struct player
{
    forward_list<position> critter_positions;
    forward_list<int> birth_rates;
    forward_list<int> death_rates;

    // TODO: Add attributes!
};

#endif