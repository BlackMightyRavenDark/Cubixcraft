#include "BlockDirt.h"

CBlockDirt::CBlockDirt(CWorld* world, POINT* textureCoord):
                 CBlock(world, BLOCK_DIRT_ID, textureCoord, L"Dirt"){}
