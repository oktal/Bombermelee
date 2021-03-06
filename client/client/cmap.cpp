#include "cmap.h"

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

CMap::CMap()
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
                m_map[i][j] = Floor;
            }
        }
    }
}

/**
  Get a block
*/
BlockType CMap::getBlock(unsigned x, unsigned y)
{
    if (x < MAP_WIDTH && y < MAP_HEIGHT)
    {
        return m_map[x][y];
    }
    else
    {
        return Floor;
    }
}

/**
  Set a block
*/
void CMap::setBlock(unsigned x, unsigned y, BlockType block)
{
    if (x < MAP_WIDTH && y < MAP_HEIGHT)
    {
        m_map[x][y] = block;
    }
}

/**
  Set the map from a string argument
*/
void CMap::setMap(const std::string &map)
{
    for(unsigned x = 0; x < MAP_WIDTH; x++)
    {
        for(unsigned y = 0; y < MAP_HEIGHT; y++)
        {
            switch(map[(x+y*MAP_WIDTH)])
            {
            case 'W':
                m_map[x][y] = Wall;
                break;
            case 'B':
                m_map[x][y] = Box;
                break;
            case '-':
               m_map[x][y] = Floor;
                break;
            case '+':
                m_map[x][y] = BonusBox;
                break;
            case 'P':
                m_map[x][y] = Player;
                break;
            case 'Q':
                m_map[x][y] = Bomb;
                break;
            case 'N':
            default:
                break;
            }
        }
    }
}
