gcc -c src\block.cpp -o obj\block.o
gcc -c src\chunk.cpp -o obj\chunk.o
gcc -c src\collisionbox.cpp -o obj\collisionbox.o
gcc -c src\font.cpp -o obj\font.o
gcc -c src\frustum.cpp -o obj\frustum.o
gcc -c src\main.cpp -o obj\main.o
gcc -c src\objectselector.cpp -o obj\objectselector.o
gcc -c src\player.cpp -o obj\player.o
gcc -c src\tesselator.cpp -o obj\tesselator.o
gcc -c src\textures.cpp -o obj\textures.o
gcc -c src\timers.cpp -o obj\timers.o
gcc -c src\world.cpp -o obj\world.o
gcc -c src\worldrenderer.cpp -o obj\worldrenderer.o
gcc -o bin\Debug\Cubixcraft.exe obj\block.o obj\chunk.o obj\collisionbox.o obj\font.o obj\frustum.o obj\main.o obj\objectselector.o obj\player.o obj\tesselator.o obj\textures.o obj\timers.o obj\world.o obj\worldrenderer.o obj\SOIL\SOIL.o obj\SOIL\image_DXT.o obj\SOIL\image_helper.o obj\SOIL\stb_image_aug.o -static -s -lopengl32 -lglu32 -lgdi32 -lstdc++
pause