#ifndef CMAPGEN_H
#define CMAPGEN_H

#include <string>

static const unsigned MAP_WIDTH = 15;
static const unsigned MAP_HEIGHT  = 15;

static const unsigned SPACE = 2; // Minimal number of space to get from border of the map
static const unsigned PRC_BOX = 70; // Percentage of chance to get a 'Box'
static const unsigned PRC_BONUS = 10; // Percentage of chance to get a 'Bonus'

class CMapGen
{
    public:
        CMapGen();
        ~CMapGen();

        enum BlockType
        {
            Wall = 1,
            Box,
            Floor,
            Bonus,
            Bomb,
            Player,
            None
        };

        void generateMap();
        std::string getMap();


    private:
        BlockType m_map[MAP_WIDTH][MAP_HEIGHT];
};

#endif // CMAPGEN_H
