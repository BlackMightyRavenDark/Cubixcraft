package game.cubixcraft.block;

import game.cubixcraft.world.World;
import game.cubixcraft.utils.Vec2i;

public class BlockDirt extends Block {

    public BlockDirt(World world, Vec2i textureCoord) {
        super(world, BLOCK_DIRT_ID, textureCoord, "Dirt");
    }
}
