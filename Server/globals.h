#ifndef __GLOBALS_H__
#define __GLOBALS_H__

int MAP_WIDTH = 1000;
int MAP_HEIGHT = 1000;

// This 2D array of bools tracks valid positions on the map.
bool **VALID_MAP;

// This 2D array of LLs tracks pheromones at each place
// Use a ushort for each player (which allows 8 players)
long long **PHEROMONE_MAP;
// Only write to this map! Then copy over.
long long **PHEROMONE_MAP_NEXT;

// This 2D array of LLs tracks how many agents exist at each place
// Use a ushort for each player (which allows 8 players)
long long **CRITTER_MAP;
// Only write to this map! Then copy over.
long long **CRITTER_MAP_NEXT;

#endif
