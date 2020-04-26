#include <vector>

class Chunk;
class Tesselator;
class World;

class WorldRenderer {
public:
    World* worldObj;

    int xChunks;
    int yChunks;
    int zChunks;

    std::vector<Chunk*> chunks;

    int chunksCount;
    int chunksRendered;
    int totalBlocksRendered;
    int totalBlocksPossible;

    WorldRenderer(World* world);
    ~WorldRenderer();
    int Render();
    int RenderBlockSide(Tesselator* t, float x, float y, float z, int side);
    void RebuildAllChunks();
    void BlockChanged(int x, int y, int z);
    int GetArrayId(int x, int y, int z);

};
