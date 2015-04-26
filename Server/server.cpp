#include "server.h"

int main()
{
    srand(time(NULL));

    // TODO: Networky stuff!
    generateMap();
    DEBUG_SETUP();
    for(int i = 0; i < 10; i++)
    {
        generateNextStep();
    }
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

        int hoff = rand() % (MAP_WIDTH - 10) + 1;
        int voff = rand() % (MAP_HEIGHT - 10) + 1;
        auto crit = it->critter_positions.before_begin();

        for(int j = 0; j < 300; j++)
        {
            int x = voff + rand() % 10;
            int y = hoff + rand() % 10;

            crit = it->critter_positions.emplace_after(crit, x, y);
        }

        auto pop = it->pop_changes.before_begin();
        for(int j = 0; j < 10; j++)
        {
            pop = it->pop_changes.insert_after(pop, rand()%6-rand()%6);
        }

        it->herd_mentality = rand()%10+1;
        it->prey_mentality = rand()%10+1;
    }
}

void generateNextStep()
{
    decayPheromones();
    moveCritters();
    handleConflicts();
}

void decayPheromones()
{
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            for(int k = 0; k < NUM_PLAYERS; k++)
            {
                PHEROMONE_MAP_NEXT[i][j][k] = PHEROMONE_MAP_NEXT[i][j][k] >> 1;
            }
        }
    }
}

void moveCritters()
{
    int playerNum = 0;
    for(auto it = PLAYERS.begin();
        it != PLAYERS.end();
        it++)
    {
        auto end = it->critter_positions.end();
        for(auto critter = it->critter_positions.begin();
            critter != end;
            critter++)
        {
            moveCritter(*critter, it->herd_mentality, it->prey_mentality, playerNum);
        }

        playerNum++;
    }
}

int getMaxPheromones(int x, int y, int excludePlayer)
{
    int max = 0;

    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        if(i != excludePlayer && PHEROMONE_MAP[x][y][i] > max)
            max = PHEROMONE_MAP[x][y][i];
    }

    return max;
}

void moveCritter(position &p, int herd, int pred, int playerNum)
{
    square ul(p.x-1, p.y-1);
    square u(p.x, p.y-1);
    square ur(p.x+1, p.y-1);
    square l(p.x-1, p.y);
    square c(p.x, p.y);
    square r(p.x+1, p.y);
    square dl(p.x-1, p.y+1);
    square d(p.x, p.y+1);
    square dr(p.x+1, p.y+1);

    forward_list<square> squares;
    auto it = squares.before_begin();

    c.odds = herd * PHEROMONE_MAP[c.x][c.y][playerNum]
             + pred * getMaxPheromones(c.x, c.y, playerNum);
    int min = c.odds;
    it = squares.insert_after(it, c);
    if(VALID_MAP[ul.x][ul.y])
    {
        ul.odds = herd * PHEROMONE_MAP[ul.x][ul.y][playerNum]
                 + pred * getMaxPheromones(ul.x, ul.y, playerNum);
        it = squares.insert_after(it, ul);

        if(ul.odds < min)
            min = ul.odds;
    }
    if(VALID_MAP[u.x][u.y])
    {
        u.odds = herd * PHEROMONE_MAP[u.x][u.y][playerNum]
                 + pred * getMaxPheromones(u.x, u.y, playerNum);
        it = squares.insert_after(it, u);

        if(u.odds < min)
            min = u.odds;
    }
    if(VALID_MAP[ur.x][ur.y])
    {
        ur.odds = herd * PHEROMONE_MAP[ur.x][ur.y][playerNum]
                 + pred * getMaxPheromones(ur.x, ur.y, playerNum);
        it = squares.insert_after(it, ur);

        if(ur.odds < min)
            min = ur.odds;
    }
    if(VALID_MAP[l.x][l.y])
    {
        l.odds = herd * PHEROMONE_MAP[l.x][l.y][playerNum]
                 + pred * getMaxPheromones(l.x, l.y, playerNum);
        it = squares.insert_after(it, l);

        if(l.odds < min)
            min = l.odds;
    }
    if(VALID_MAP[r.x][r.y])
    {
        r.odds = herd * PHEROMONE_MAP[r.x][r.y][playerNum]
                 + pred * getMaxPheromones(r.x, r.y, playerNum);
        it = squares.insert_after(it, r);

        if(r.odds < min)
            min = r.odds;
    }
    if(VALID_MAP[dl.x][dl.y])
    {
        dl.odds = herd * PHEROMONE_MAP[dl.x][dl.y][playerNum]
                 + pred * getMaxPheromones(dl.x, dl.y, playerNum);
        it = squares.insert_after(it, dl);

        if(dl.odds < min)
            min = dl.odds;
    }
    if(VALID_MAP[d.x][d.y])
    {
        d.odds = herd * PHEROMONE_MAP[d.x][d.y][playerNum]
                 + pred * getMaxPheromones(d.x, d.y, playerNum);
        it = squares.insert_after(it, d);

        if(d.odds < min)
            min = d.odds;
    }
    if(VALID_MAP[dr.x][dr.y])
    {
        dr.odds = herd * PHEROMONE_MAP[dr.x][dr.y][playerNum]
                 + pred * getMaxPheromones(dr.x, dr.y, playerNum);
        it = squares.insert_after(it, dr);

        if(dr.odds < min)
            min = dr.odds;
    }

    float total = 0.000001;
    for(it = squares.begin(); it != squares.end(); ++it)
    {
        total += it->odds -= min;
    }

    float roul = (rand() % 1000) / 1000.0f;
    float running = 0;

    for(it = squares.begin(); it != squares.end(); ++it)
    {
        running += it->odds / total;
        if(running >= roul)
            break;
    }

    if(it != squares.end())
    {
        CRITTER_MAP_NEXT[p.x][p.y][playerNum]--;
        p.x = it->x;
        p.y = it->y;
        CRITTER_MAP_NEXT[p.x][p.y][playerNum]++;
    }

    PHEROMONE_MAP_NEXT[p.x][p.y][playerNum] = (PHEROMONE_MAP_NEXT[p.x][p.y][playerNum] >> 1) | 128;
}

void handleConflicts()
{
    int playerNum = 0;
    for(auto it = PLAYERS.begin();
        it != PLAYERS.end();
        it++)
    {
        auto end = it->critter_positions.end();
        for(auto critter = it->critter_positions.begin();
            critter != end;
            critter++)
        {
            // TODO: Handle conflicts!...somehow.
            // This will modify BOTH lists of critters, so we'll need to be
            //  a little clever.
        }

        playerNum++;
    }
}

void generateMap()
{
    VALID_MAP = new bool*[MAP_HEIGHT];
    PHEROMONE_MAP = new unsigned char**[MAP_HEIGHT];
    PHEROMONE_MAP_NEXT = new unsigned char**[MAP_HEIGHT];
    CRITTER_MAP = new unsigned char**[MAP_HEIGHT];
    CRITTER_MAP_NEXT = new unsigned char**[MAP_HEIGHT];

    // TODO: Eventually generate a map
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        VALID_MAP[i] = new bool[MAP_WIDTH];
        PHEROMONE_MAP[i] = new unsigned char*[MAP_WIDTH];
        PHEROMONE_MAP_NEXT[i] = new unsigned char*[MAP_WIDTH];
        CRITTER_MAP[i] = new unsigned char*[MAP_WIDTH];
        CRITTER_MAP_NEXT[i] = new unsigned char*[MAP_WIDTH];

        for(int j = 0; j < MAP_WIDTH; j++)
        {
            VALID_MAP[i][j] = (i > 0 && i < MAP_HEIGHT - 1)
                            && (j > 0 && j < MAP_WIDTH - 1);
            PHEROMONE_MAP[i][j] = new unsigned char[NUM_PLAYERS];
            PHEROMONE_MAP_NEXT[i][j] = new unsigned char[NUM_PLAYERS];
            CRITTER_MAP[i][j] = new unsigned char[NUM_PLAYERS];
            CRITTER_MAP_NEXT[i][j] = new unsigned char[NUM_PLAYERS];

            for(int k = 0; k < NUM_PLAYERS; k++)
            {
                PHEROMONE_MAP[i][j][k] = 0;
                PHEROMONE_MAP_NEXT[i][j][k] = 0;
                CRITTER_MAP[i][j][k] = 0;
                CRITTER_MAP_NEXT[i][j][k] = 0;
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
            for(int k = 0; k < NUM_PLAYERS; k++)
            {
                PHEROMONE_MAP[i][j][k] = PHEROMONE_MAP_NEXT[i][j][k];
                CRITTER_MAP[i][j][k] = CRITTER_MAP_NEXT[i][j][k];
            }
        }
    }
}

void exterminate()
{
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            delete [] PHEROMONE_MAP[i][j];
            delete [] PHEROMONE_MAP_NEXT[i][j];
            delete [] CRITTER_MAP[i][j];
            delete [] CRITTER_MAP_NEXT[i][j];
        }

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
