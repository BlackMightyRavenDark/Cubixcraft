package game.cubixcraft.block;

import game.cubixcraft.world.World;
import game.cubixcraft.utils.Vec2i;

public class BlockAir extends Block {

    public BlockAir(World world) {
        super(world, BLOCK_AIR_ID, new Vec2i(0, 0), "Air");
    }
}
