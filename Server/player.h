#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <forward_list>

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
    forward_list<int> pop_changes;

    int herd_mentality; // -5 to +5
    int prey_mentality; // -5 to +5
    int vision; // 1 to 5
    int flight_mentality; // -5 to +5
};

#endif
