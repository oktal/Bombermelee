#include "cmap.h"

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
                m_map[i][j] = None;
            }
        }
    }
}

BlockType CMap::getBlock(unsigned x, unsigned y)
{
    if (x < MAP_WIDTH && y < MAP_HEIGHT)
    {
        return m_map[x][y];
    }
    else
    {
        return None;
    }
}

void CMap::setBlock(unsigned x, unsigned y, BlockType block)
{
    if (x < MAP_WIDTH && y < MAP_HEIGHT)
    {
        m_map[x][y] = block;
    }
}
