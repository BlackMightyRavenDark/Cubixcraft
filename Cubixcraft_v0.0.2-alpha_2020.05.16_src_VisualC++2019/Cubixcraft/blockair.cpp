#include "blockair.h"
#include "world.h"

CBlockAir::CBlockAir(CWorld* world) : CBlock(world, BLOCK_AIR_ID, new POINT{ 0,0 }, L"Air"){}
