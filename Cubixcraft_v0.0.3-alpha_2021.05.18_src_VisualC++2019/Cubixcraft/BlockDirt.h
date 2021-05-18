#include "block.h"
#include <windows.h>

class CBlockDirt : public CBlock {

public:
    CBlockDirt(CWorld* world, POINT* textureCoord);
};
