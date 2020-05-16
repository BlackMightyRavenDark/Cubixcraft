#include "blockdirt.h"

CBlockDirt::CBlockDirt(CWorld* world, POINT* texCoord):
                 CBlock(world, BLOCK_DIRT_ID, texCoord, L"Dirt"){}
