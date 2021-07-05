#include "libs.h"

int main(void)
{
	Game game("Snake 3D", 500, 500);

	GLuint floorTex = readTexture("wood.png");
	Material floorMat;
	Floor floor;

	game.addTexture(&floorTex);
	floorMat.setDiffuseTex(*game.getTexture(0));
	game.addMaterial(&floorMat);

	// Game loop
	while (!glfwWindowShouldClose(game.getWindow()))
	{
		game.drawScene();
		glfwPollEvents();
	}

	// End game
	game.~Game();
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
