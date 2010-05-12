#ifndef CMAP_H
#define CMAP_H

#include <string>

static const unsigned MAP_WIDTH = 15;
static const unsigned MAP_HEIGHT = 15;
static const unsigned BLOCK_SIZE = 34;

enum BlockType
{
    Wall = 1,
    Box,
    Floor,
    BonusBox,
    Bonus,
    Bomb,
    Player
};

/**
  @class CMap represents the whole map
*/
class CMap
{
public:
    CMap();

    BlockType getBlock(unsigned x, unsigned y);
    void setBlock(unsigned x, unsigned y, BlockType block);
    void setMap(const std::string &map);

private:
    BlockType m_map[MAP_WIDTH][MAP_HEIGHT];

};

#endif // CMAP_H
