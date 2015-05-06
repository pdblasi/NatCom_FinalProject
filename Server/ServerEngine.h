#ifndef __SERVER_ENGINE_H__
#define __SERVER_ENGINE_H__

#include <algorithm>
#include <forward_list>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>
#include "Globals.h"
#include "player.h"
#include "ServerComm.h"

using namespace std;

class ServerEngine
{
    public:
    ServerEngine(ServerComm *comm);
    ~ServerEngine();
    bool generateNextStep();
    
    private:
        ServerComm *m_comm;
        forward_list<player> PLAYERS;
        int NUM_PLAYERS;
        int m_totalPopulation;
        // Tracks valid positions on the map
        bool **VALID_MAP;
        // Tracks each position's pheromones for each player
        unsigned char ***PHEROMONE_MAP;
        // Pheromone map's next step
        unsigned char ***PHEROMONE_MAP_NEXT;
        // Tracks each position's critters for each player
        unsigned char ***CRITTER_MAP;
        // Critter map's next step
        unsigned char ***CRITTER_MAP_NEXT;
        int MAP_HEIGHT;
        int MAP_WIDTH;

    // Set up the VALID_MAP bool array
    void generateMap();
    // Copy NEXT maps to counterparts
    void stepMapsForward();

    void DEBUG_SETUP();

    void updatePlayerStatuses();
    void sendPlayerUpdates();

    void decayPheromones();
    void moveCritters();
    void moveCritter(position &p, int herd, int pred, int playerNum);
    int getMaxPheromones(int x, int y, int excludePlayer);

    void handleConflicts();
    void removeCritter(int playerNum, position critter);
    bool trialByCombat(const player &predator, const player &prey);

    int checkWinner();

    void loadConfigData();

    string buildMap();
    string buildStats();

    struct square
    {
        int x, y;
        int odds;

        square(int _x, int _y): x(_x), y(_y) { };
    };
};

#endif
