#include <Windows.h>
#include "block.h"

class CBlockGrass : public CBlock {
public:
	CBlockGrass(CWorld* world, POINT* textureCoord);

	POINT* GetTextureCoordinates(int sideId) override;
};
