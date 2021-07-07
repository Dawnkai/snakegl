#define GLM_FORCE_RADIANS

#include "libs.h"
#include "floor.h"

int main(void)
{
	App *app = new App(500, 500, "Snake");

	Model *apple = new Model("apple.obj", "apple.png");
	apple->scale(glm::vec3(4.0f, 4.0f, 4.0f));
	app->addModel(apple);

	Model *floor = new Model(myFloorVertices, myFloorTexCoords, myFloorNormals, myFloorColors, myFloorVertexCount);
	floor->setTexture("wood.png");
	app->addModel(floor);

	//Main loop
	while (!glfwWindowShouldClose(app->getWindow()))
	{
		app->drawScene();
		app->updateInput();
	}

	delete apple;
	delete app;
}

