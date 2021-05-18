#include "BlockGrass.h"
#include "world.h"

CBlockGrass::CBlockGrass(CWorld* world, POINT* textureCoord) :
	CBlock(world, BLOCK_GRASS_ID, textureCoord, L"Grass") {}

POINT* CBlockGrass::GetTextureCoordinates(int sideId)
{
	switch (sideId)
	{
		case BLOCK_SIDE_TOP:
			return new POINT{ 0, 0 };
		case BLOCK_SIDE_BOTTOM:
			return new POINT{ 2,0 };
		default:
			return new POINT{ 1,0 };
	}
}
