package game.cubixcraft.block;

import game.cubixcraft.world.World;
import game.cubixcraft.utils.Vec2i;

public class BlockGrass extends Block {

    public BlockGrass(World world, Vec2i textureCoord) {
        super(world, BLOCK_GRASS_ID, textureCoord, "Grass");
    }

    @Override
    public Vec2i getTextureCoordinates(int sideId) {
        switch (sideId) {
            case BLOCK_SIDE_TOP:
                return new Vec2i(0,0);
            case BLOCK_SIDE_BOTTOM:
                return new Vec2i(2, 0);
            default:
                return new Vec2i(1, 0);
        }
    }
}