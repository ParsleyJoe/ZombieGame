#include <raylib.h>
#include <raymath.h>

#include "game.hpp"

int main()
{
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(800, 600, "Zombies..., Are Coming");

	Game game;
	while (!WindowShouldClose()) {
		game.update();
		
		BeginDrawing();
		ClearBackground(GREEN);
		game.draw();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
