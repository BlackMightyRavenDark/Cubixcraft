#include "blockgrass.h"
#include "world.h"

CBlockGrass::CBlockGrass(CWorld* world, POINT* textureCoord) :
	CBlock(world, BLOCK_GRASS_ID, textureCoord, L"Grass") {}

POINT CBlockGrass::GetTextureCoordinates(int side)
{
	switch (side)
	{
		case BLOCK_SIDE_TOP:
			return POINT{ 0, 0 };
		case BLOCK_SIDE_BOTTOM:
			return POINT{ 2,0 };
		default:
			return POINT{ 1,0 };
	}
}
