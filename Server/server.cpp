#include "server.h"

int main()
{
    return 0;
}

void stepMapsForward()
{
    for(int i = 0; i < MAP_WIDTH; i++)
    {
        for(int j = 0; j < MAP_HEIGHT; j++)
        {
            PHEROMONE_MAP[i][j] = PHEROMONE_MAP_NEXT[i][j];
            CRITTER_MAP[i][j] = CRITTER_MAP_NEXT[i][j];
        }
    }
}
