#include "libs.h"
#include "floor.h"

int main(void)
{
	Game game("Snake 3D", 500, 500);

	glm::vec3 *mainLight = new glm::vec3(0.0f, 0.0f, 2.0f);
	Model apple("apple.png", "apple.obj");

	game.addModel(&apple);
	game.addLight(mainLight);

	// Game loop
	while (!glfwWindowShouldClose(game.getWindow()))
	{
		game.drawScene();
		glfwPollEvents();
		game.updateInput();
	}

	// End game
	game.~Game();
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
