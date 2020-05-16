#include "block.h"
#include <windows.h>
#include <GL/gl.h>

class CWorld;

class CBlockDirt : public CBlock {

public:
    CBlockDirt(CWorld* world, POINT* texCoord);
};
