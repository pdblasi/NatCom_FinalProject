#include "server.h"

int main()
{
    srand(time(NULL));

    // TODO: Networky stuff!
    generateMap();
    DEBUG_SETUP();
    // TODO: Algorithmic stuff!
    // TODO: Networky stuff!
    exterminate();

    return 0;
}

void DEBUG_SETUP()
{
    auto it = PLAYERS.before_begin();

    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        it = PLAYERS.emplace_after(it);

        int hoff = rand() % (MAP_WIDTH - 10);
        int voff = rand() % (MAP_HEIGHT - 10);
        auto crit = it->critter_positions.before_begin();

        for(int j = 0; j < 30; j++)
        {
            int x = voff + rand() % 10;
            int y = hoff + rand() % 10;

            crit = it->critter_positions.emplace_after(crit, x, y);
        }
    }
}

void generateMap()
{
    VALID_MAP = new bool*[MAP_HEIGHT];
    PHEROMONE_MAP = new char**[MAP_HEIGHT];
    PHEROMONE_MAP_NEXT = new char**[MAP_HEIGHT];
    CRITTER_MAP = new char**[MAP_HEIGHT];
    CRITTER_MAP_NEXT = new char**[MAP_HEIGHT];

    // TODO: Eventually generate a map
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        VALID_MAP[i] = new bool[MAP_WIDTH];
        PHEROMONE_MAP[i] = new char*[MAP_WIDTH];
        PHEROMONE_MAP_NEXT[i] = new char*[MAP_WIDTH];
        CRITTER_MAP[i] = new char*[MAP_WIDTH];
        CRITTER_MAP_NEXT[i] = new char*[MAP_WIDTH];

        for(int j = 0; j < MAP_WIDTH; j++)
        {
            VALID_MAP[i][j] = true;
            PHEROMONE_MAP[i][j] = new char[NUM_PLAYERS];
            PHEROMONE_MAP_NEXT[i][j] = new char[NUM_PLAYERS];
            CRITTER_MAP[i][j] = new char[NUM_PLAYERS];
            CRITTER_MAP_NEXT[i][j] = new char[NUM_PLAYERS];

            for(int k = 0; k < NUM_PLAYERS; k++)
            {
                PHEROMONE_MAP[i][j] = 0;
                PHEROMONE_MAP_NEXT[i][j] = 0;
                CRITTER_MAP[i][j] = 0;
                CRITTER_MAP_NEXT[i][j] = 0;
            }
        }
    }
}

void stepMapsForward()
{
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            PHEROMONE_MAP[i][j] = PHEROMONE_MAP_NEXT[i][j];
            CRITTER_MAP[i][j] = CRITTER_MAP_NEXT[i][j];
        }
    }
}

void exterminate()
{
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        delete [] VALID_MAP[i];
        delete [] PHEROMONE_MAP[i];
        delete [] PHEROMONE_MAP_NEXT[i];
        delete [] CRITTER_MAP[i];
        delete [] CRITTER_MAP_NEXT[i];
    }

    delete [] VALID_MAP;
    delete [] PHEROMONE_MAP;
    delete [] PHEROMONE_MAP_NEXT;
    delete [] CRITTER_MAP;
    delete [] CRITTER_MAP_NEXT;
}
