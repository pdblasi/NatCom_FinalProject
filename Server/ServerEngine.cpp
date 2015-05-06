#include "ServerEngine.h"

// ~~~ Public Methods ~~~

ServerEngine::ServerEngine(ServerComm *comm)
{
    srand(time(NULL));

    m_comm = comm;
    NUM_PLAYERS = m_comm->numPlayers();

    generateMap();
    loadConfigData();
    DEBUG_SETUP();
}

ServerEngine::~ServerEngine()
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

bool ServerEngine::generateNextStep()
{
    updatePlayerStatuses();
    decayPheromones();
    moveCritters();
    handleConflicts();
    stepMapsForward();

    int winner = checkWinner();
    if(winner != -1)
    {
        string win = to_string(winner);
        m_comm->broadcastPacket(HEADER_TYPE_VICTORY, win.length(), (void*)win.c_str());

        return true;
    }

    sendPlayerUpdates();

    return false;
}

// ~~~ Private Methods ~~~

int ServerEngine::checkWinner()
{
    int i = 0;

    for(auto player : PLAYERS)
    {
        if(float(player.tot_pop) / m_totalPopulation > 0.799)
        {
            return i;
        }

        i++;
    }

    return -1;
}

void ServerEngine::DEBUG_SETUP()
{
    auto it = PLAYERS.before_begin();

    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        it = PLAYERS.emplace_after(it);

        int hoff = rand() % (MAP_WIDTH - 10) + 1;
        int voff = rand() % (MAP_HEIGHT - 10) + 1;
        auto crit = it->critter_positions.before_begin();

        for(int j = 0; j < 3000; j++)
        {
            int x = voff + rand() % 10;
            int y = hoff + rand() % 10;

            crit = it->critter_positions.emplace_after(crit, x, y);
        }

        it->herd_mentality = rand()%11-5;
        it->prey_mentality = rand()%11-5;
    }
}

void ServerEngine::updatePlayerStatuses()
{
    auto it = m_comm->playerList();
    auto p = PLAYERS.begin();
    int i = 0;

    while(i < NUM_PLAYERS)
    {
        char msgType;
        int *l = (int*)m_comm->readPacket(*it, msgType);

        if(l != NULL)
        {
            int len = *l;
            delete [] l;

            char *m = (char*)m_comm->readPacket(*it, msgType);

            if(m != NULL)
            {
                char *msg = new char[len+1];
                strncpy(msg, m, len);
                delete [] m;
                msg[len] = 0;
                string message(msg);
                delete [] msg;

                stringstream strm;
                strm.str(message);
                strm >> p->pop_change
                    >> p->herd_mentality
                    >> p->prey_mentality
                    >> p->flight_mentality
                    >> p->lifespan
                    >> p->vision;

                p->tot_pop += p->pop_change;
                m_totalPopulation += p->pop_change;
            }
        }

        it++;
        p++;
        i++;
    }
}

string ServerEngine::buildMap()
{
    string map = "[";
    for(int i = 0; i < MAP_HEIGHT; i++)
    {
        if(i > 0)
            map += ",";

        map += "[";
        for(int j = 0; j < MAP_WIDTH; j++)
        {
            if(j > 0)
                map += ",";

            if(!VALID_MAP[i][j])
            {
                map += "-2";
                continue;
            }

            int max = 0;
            int midx = -1;

            for(int k = 0; k < NUM_PLAYERS; k++)
            {
                if(CRITTER_MAP[i][j][k] > max)
                {
                    midx = k;
                    max = CRITTER_MAP[i][j][k];
                }
            }

            map += to_string(midx);
        }
        map += "]";
    }
    map += "]";

    return map;
}

string ServerEngine::buildStats()
{
    string stats = "[";
    bool comma = false;

    for(auto player : PLAYERS)
    {
        if(comma)
            stats += ",";
        comma = true;
        stats += "[";
        stats += to_string(player.prey_mentality) + ",";
        stats += to_string(player.herd_mentality) + ",";
        stats += to_string(player.flight_mentality) + ",";
        stats += to_string(player.lifespan) + ",";
        stats += to_string(player.vision) + "]";
    }

    stats += "]";
    return stats;
}

void ServerEngine::sendPlayerUpdates()
{
    string map = buildMap();
    string stats = buildStats();

    auto it = m_comm->playerList();
    for(auto player : PLAYERS)
    {
        player.tot_pop += player.pop_change;

        string msg = "[" + to_string(player.tot_pop) + "," + map + "," + stats + "]";

        m_comm->sendPacket(*it, HEADER_TYPE_ENGINE, msg.length(), (void*)msg.c_str());

        it++;
    }
}

void ServerEngine::decayPheromones()
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

void ServerEngine::moveCritters()
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

int ServerEngine::getMaxPheromones(int x, int y, int excludePlayer)
{
    int max = 0;

    for(int i = 0; i < NUM_PLAYERS; i++)
    {
        if(i != excludePlayer && PHEROMONE_MAP[x][y][i] > max)
            max = PHEROMONE_MAP[x][y][i];
    }

    return max;
}

void ServerEngine::moveCritter(position &p, int herd, int pred, int playerNum)
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

    int abs_min = 2550; // 255 * 5 * 2
    c.odds = herd * PHEROMONE_MAP[c.x][c.y][playerNum]
             + pred * getMaxPheromones(c.x, c.y, playerNum)
             + abs_min;
    float tot = c.odds;
    it = squares.insert_after(it, c);
    if(VALID_MAP[ul.x][ul.y])
    {
        ul.odds = herd * PHEROMONE_MAP[ul.x][ul.y][playerNum]
                 + pred * getMaxPheromones(ul.x, ul.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, ul);

        tot += ul.odds;
    }
    if(VALID_MAP[u.x][u.y])
    {
        u.odds = herd * PHEROMONE_MAP[u.x][u.y][playerNum]
                 + pred * getMaxPheromones(u.x, u.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, u);

        tot += u.odds;
    }
    if(VALID_MAP[ur.x][ur.y])
    {
        ur.odds = herd * PHEROMONE_MAP[ur.x][ur.y][playerNum]
                 + pred * getMaxPheromones(ur.x, ur.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, ur);

        tot += ur.odds;
    }
    if(VALID_MAP[l.x][l.y])
    {
        l.odds = herd * PHEROMONE_MAP[l.x][l.y][playerNum]
                 + pred * getMaxPheromones(l.x, l.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, l);

        tot += l.odds;
    }
    if(VALID_MAP[r.x][r.y])
    {
        r.odds = herd * PHEROMONE_MAP[r.x][r.y][playerNum]
                 + pred * getMaxPheromones(r.x, r.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, r);

        tot += r.odds;
    }
    if(VALID_MAP[dl.x][dl.y])
    {
        dl.odds = herd * PHEROMONE_MAP[dl.x][dl.y][playerNum]
                 + pred * getMaxPheromones(dl.x, dl.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, dl);

        tot += dl.odds;
    }
    if(VALID_MAP[d.x][d.y])
    {
        d.odds = herd * PHEROMONE_MAP[d.x][d.y][playerNum]
                 + pred * getMaxPheromones(d.x, d.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, d);

        tot += d.odds;
    }
    if(VALID_MAP[dr.x][dr.y])
    {
        dr.odds = herd * PHEROMONE_MAP[dr.x][dr.y][playerNum]
                 + pred * getMaxPheromones(dr.x, dr.y, playerNum)
                 + abs_min;
        it = squares.insert_after(it, dr);

        tot += dr.odds;
    }

    float roul = (rand() % 1000) / 1000.0f;
    float running = 0;

    for(it = squares.begin(); it != squares.end(); ++it)
    {
        running += it->odds / tot;
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

void ServerEngine::handleConflicts()
{
    int playerNum = 0;

    for(auto player : PLAYERS)
    {
        player.pop_change = 0;
    }

    for(auto it = PLAYERS.begin();
        it != PLAYERS.end();
        it++)
    {
        for(auto critter = it->critter_positions.begin();
            critter != it->critter_positions.end();
            critter++)
        {
            unsigned char *num_runs = CRITTER_MAP[critter->x][critter->y];

            for(int i = playerNum+1; i < NUM_PLAYERS; i++)
            {
                unsigned char us = num_runs[playerNum];
                unsigned char them = num_runs[i];

                if(them > 0)
                {
                    unsigned char min;
                    if(them > us)
                        min = us;
                    else
                        min = them;

                    auto it2 = next(it, i-playerNum);
                    if(it2->prey_mentality > it->prey_mentality)
                    {
                        for(int n = 0; n < min; n++)
                        {
                            if((rand()%1000)/1000.0 < it->vision)
                            {
                                if(trialByCombat(*it, *it2))
                                {
                                    removeCritter(i, *critter);
                                }
                                else
                                {
                                    removeCritter(playerNum, *critter);
                                }
                            }
                        }
                    }
                    else
                    {
                        for(int n = 0; n < min; n++)
                        {
                            if((rand()%1000)/1000.0 < it->vision)
                            {
                                if(trialByCombat(*it2, *it))
                                {
                                    removeCritter(playerNum, *critter);
                                }
                                else
                                {
                                    removeCritter(i, *critter);
                                }
                            }
                        }
                    }
                }
            }
        }

        playerNum++;
    }
}

bool ServerEngine::trialByCombat(const player &predator, const player &prey)
{
    int chance = rand() % int(3 * predator.vision) + 1;
    float win = (rand() % 1000) / 1000.0f;

    if(prey.vision > chance)
    {
        if(predator.flight_mentality < 0)
        {
            // strength
            if( (-prey.flight_mentality + 5) / 10 > win )
            {
                // kill pred
                return false;
            }
            else
            {
                // kill prey
                return true;
            }
        }
        else
        {
            // speed
            if( (prey.flight_mentality + 5) / 10 > win)
            {
                // kill pred
                return false;
            }
            else
            {
                // kill prey
                return true;
            }
        }
    }
    else
    {
        if(prey.flight_mentality < 0)
        {
            // strength
            if( (-predator.flight_mentality + 5) / 10 > win)
            {
                // kill prey
                return true;
            }
            else
            {
                // kill pred
                return false;
            }
        }
        else
        {
            // speed
            if( (predator.flight_mentality + 5) / 10 > win)
            {
                // kill prey
                return true;
            }
            else
            {
                // kill pred
                return false;
            }
        }
    }
}

void ServerEngine::removeCritter(int playerNum, position critter)
{
    auto player = PLAYERS.begin();

    advance(player, playerNum);

    for(auto c1 = player->critter_positions.before_begin(),
             c2 = player->critter_positions.begin();
        c2 != player->critter_positions.end();
        ++c1, ++c2)
    {
        if((*c2) == critter)
        {
            player->critter_positions.erase_after(c1);
            break;
        }
    }

    player->pop_change--;
}

void ServerEngine::generateMap()
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

void ServerEngine::stepMapsForward()
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

void ServerEngine::loadConfigData()
{
    ifstream fin("server.config");

    MAP_HEIGHT = 1000;
    MAP_WIDTH = 1000;

    if(fin.is_open())
    {
        string line;

        while(getline(fin, line))
        {
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
            replace(line.begin(), line.end(), '=', ' ');

            int pos = line.find("=");
            string prop = line.substr(0,pos);
            pos++;
            string val = line.substr(pos, line.length()-pos);

            if(prop == "mapheight")
            {
                MAP_HEIGHT = atoi(val.c_str());
            }
            else if(prop == "mapwidth")
            {
                MAP_WIDTH = atoi(val.c_str());
            }
        }

        fin.close();
    }
    else
    {
        ofstream fout("server.config");

        fout << "mapheight=" << MAP_HEIGHT << endl;
        fout << "mapwidth=" << MAP_WIDTH << endl;

        fout.close();
    }
}
