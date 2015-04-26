#ifndef __GLOBALS_H__
#define __GLOBALS_H__

int MAP_WIDTH = 1000;
int MAP_HEIGHT = 1000;
int NUM_PLAYERS = 4;

// This 2D array of bools tracks valid positions on the map.
bool **VALID_MAP;

// This ~2D~ array of char-arrays tracks pheromones at each place
char ***PHEROMONE_MAP;
// Only write to this map! Then copy over.
char ***PHEROMONE_MAP_NEXT;

// This ~2D~ array of char-arrays tracks how many agents exist at each place
char ***CRITTER_MAP;
// Only write to this map! Then copy over.
char ***CRITTER_MAP_NEXT;

#endif
