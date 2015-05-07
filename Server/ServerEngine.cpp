#include "ServerEngine.h"

// ~~~ Public Methods ~~~

ServerEngine::ServerEngine(ServerComm *comm)
{
    srand(time(NULL));

    m_comm = comm;
    //NUM_PLAYERS = m_comm->numPlayers();
    NUM_PLAYERS = 4;

    loadConfigData();
    generateMap();
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
//    updatePlayerStatuses();
    decayPheromones();
    moveCritters();
    handleConflicts();
    stepMapsForward();

    int winner = checkWinner();
    if(winner != -1)
    {
        string win = to_string(winner);
//        m_comm->broadcastPacket(HEADER_TYPE_VICTORY, win.length(), (void*)win.c_str());

        return true;
    }

//    sendPlayerUpdates();

    return false;
}

void ServerEngine::printCurrentState(int n)
{
    cout << "Dumping state" << endl;
    cout << " Populations:" << endl;
    string filename = "state" + to_string(n) + ".dat";
    int i = 0;

    for(auto player : PLAYERS)
    {
        string fname = to_string(i) + filename;
        ofstream fout(fname.c_str());

        for(auto critter : player.critter_positions)
        {
            fout << critter.x << ' ' << critter.y << endl;
        }

        fout.close();
        i++;
        cout << "  " << i << ": " << player.tot_pop << endl;
    }
}

// ~~~ Private Methods ~~~

int ServerEngine::checkWinner()
{
    int i = 0;

    for(auto player : PLAYERS)
    {
        if(float(player.tot_pop) / m_totalPopulation > 0.799)
        {
            cout << "Winner! " << i << endl;
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

        it->spawn_y = rand() % (MAP_WIDTH - 11) + 1;
        it->spawn_x = rand() % (MAP_HEIGHT - 11) + 1;
        auto crit = it->critter_positions.before_begin();
        int size = rand() % 2000 + 1500;

        for(int j = 0; j < size; j++)
        {
            int x = it->spawn_x + rand() % 10;
            int y = it->spawn_y + rand() % 10;

            crit = it->critter_positions.emplace_after(crit, x, y);
        }

        it->herd_mentality = rand()%11-5;
        it->prey_mentality = rand()%11-5;
        it->tot_pop = size;
        it->pop_change = 0;
        it->vision = 5.0f;
//        it->vision = (rand() % 5000 + 1) / 1000;
        m_totalPopulation += size;
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
        int msgLen;
        void *msg = m_comm->readPacket(*it, msgType, msgLen);

        if(msg == NULL)
        {
            it++;
            p++;
            i++;
            continue;
        }

        char *stats = new char[msgLen+1];
        strncpy_s(stats, msgLen+1, (char*)msg, msgLen);
        stats[msgLen] = 0;
        string playerStats(stats);
        stringstream strm;
        strm.str(playerStats);

        strm >> p->pop_change
            >> p->herd_mentality
            >> p->prey_mentality
            >> p->flight_mentality
            >> p->lifespan
            >> p->vision;

        p->tot_pop += p->pop_change;
        m_totalPopulation += p->pop_change;

        delete [] (char*)msg;
        delete [] stats;

        if(p->pop_change > 0)
        {
            auto cr = p->critter_positions.before_begin();
            for(int j = 0; j < p->pop_change; j++)
            {
                int x = p->spawn_x + rand() % 10;
                int y = p->spawn_y + rand() % 10;

                p->critter_positions.emplace_after(cr, x, y);
            }
        }
        else if(p->pop_change < 0)
        {
            if(p->tot_pop > 0)
            {
                for(auto cr = next(p->critter_positions.begin(), p->tot_pop);
                    cr != p->critter_positions.end();
                    cr = p->critter_positions.erase_after(cr))
                {
                    CRITTER_MAP[cr->x][cr->y][i]--;
                }
            }
            else
            {
                p->critter_positions.clear();
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
        for(auto critter = it->critter_positions.begin();
            critter != it->critter_positions.end();
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
        auto critter = it->critter_positions.begin();
        while(critter != it->critter_positions.end())
        {
            unsigned char *num_runs = CRITTER_MAP_NEXT[critter->x][critter->y];

            for(int i = playerNum+1; i < NUM_PLAYERS; i++)
            {
                unsigned char us = num_runs[playerNum];
                unsigned char them = num_runs[i];

                if(them > 0 && us > 0)
                {
                    unsigned char min;
                    if(them > us)
                        min = us;
                    else
                        min = them;

                    auto it2 = next(it, i-playerNum);
                    int iloss = 0, ploss = 0;
                    position pos = *critter;
                    if(it2->prey_mentality > it->prey_mentality)
                    {
                        for(int n = 0; n < min; n++)
                        {
                            if((rand()%1000)/1000.0 < it->vision / 7)
                            {
                                if(trialByCombat(*it, *it2))
                                {
                                    iloss++;
                                }
                                else
                                {
                                    ploss++;
                                }
                            }
                        }
                    }
                    else
                    {
                        for(int n = 0; n < min; n++)
                        {
                            if((rand()%1000)/1000.0 < it->vision / 7)
                            {
                                if(trialByCombat(*it2, *it))
                                {
                                    ploss++;
                                }
                                else
                                {
                                    iloss++;
                                }
                            }
                        }
                    }
                    
                    for(int loss = 0; loss < iloss; loss++)
                        removeCritter(i, pos);
                    if(ploss > 0)
                        critter = removeCritter(playerNum, pos);
                        for(int loss = 1; loss < ploss; loss++)
                            removeCritter(playerNum, pos);
                }
            }
            if(critter != it->critter_positions.end())
                critter++;
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

forward_list<position>::iterator ServerEngine::removeCritter(int playerNum, position critter)
{
    auto player = PLAYERS.begin();
    forward_list<position>::iterator result;

    advance(player, playerNum);

    player->pop_change--;
    player->tot_pop--;
    CRITTER_MAP_NEXT[critter.x][critter.y][playerNum]--;

    for(auto cr = player->critter_positions.before_begin();
        next(cr) != player->critter_positions.end();
        cr++)
    {
        if(next(cr)->x == critter.x && next(cr)->y == critter.y)
        {
            result = player->critter_positions.erase_after(cr);
            break;
        }
    }

    return result;
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

    MAP_HEIGHT = 500;
    MAP_WIDTH = 500;

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
