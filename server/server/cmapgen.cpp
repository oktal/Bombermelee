#include "cmapgen.h"
#include <algorithm>
#include <iostream>
#include <ctime>
#include <time.h>

/*
This file is part of Bombermelee.

    Bombermelee is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bombermelee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bombermelee.  If not, see <http://www.gnu.org/licenses/>.
*/

CMapGen::CMapGen()
{
    for (unsigned i = 0; i < MAP_WIDTH; ++i)
    {
        for (unsigned j = 0; j < MAP_HEIGHT; ++j)
        {
            if (i % 2 != 0 && j % 2 != 0)
            {
                m_map[i][j] = Wall;
            }
            else
            {
                m_map[i][j] = None;
            }
        }
    }
}

CMapGen::~CMapGen()
{

}

void CMapGen::generateMap()
{
    const unsigned MAX = 100, MIN = 0;
    srand(time(NULL));
    unsigned nb_rand;

    for (unsigned i = 0; i < MAP_WIDTH; ++i)
    {
        for (unsigned j = 0; j < MAP_HEIGHT; ++j)
        {
            if (m_map[i][j] == Wall)
            {
                continue;
            }

            if (i >= SPACE && i < MAP_WIDTH - SPACE || j >= SPACE && j < MAP_HEIGHT - SPACE)
            {
                nb_rand = (rand()%(MAX-MIN+1))+MIN;

                if (nb_rand < PRC_BONUS)
                {
                    m_map[i][j] = Bonus;
                }
                else if (nb_rand < PRC_BOX)
                {
                    m_map[i][j] = Box;
                }
                else
                {
                    m_map[i][j] = Floor;
                }
            }
            else
            {
                m_map[i][j] = Floor;
            }
        }
    }
}

std::string CMapGen::getMap()
{
    std::string map = "";
    for (unsigned i = 0; i < MAP_WIDTH; ++i)
    {
        for (unsigned j = 0; j < MAP_HEIGHT; ++j)
        {
            switch (m_map[i][j])
            {
            case Wall:
                map += "W"; // Wall
                break;
            case Box:
                map += "B"; // Box
                break;
            case Floor:
                map += "-"; // Floor
                break;
            case Bonus:
                map += "+"; // Bonus
                break;
            case Player:
                map += "P"; // Player
                break;
            case Bomb:
                map += "Q"; // Bomb
                break;
            case None:
                map += "N"; // None
                break;
            }
        }
    }
    return map;
}
