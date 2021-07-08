#define GLM_FORCE_RADIANS

#include "libs.h"
#include "floor.h"

int main(void)
{
	App *app = new App(500, 500, "Snake");

	Model *apple = new Model("apple.obj", "apple.png");
	apple->translate(glm::vec3(0.0f, 3.0f, 0.0f));
	apple->scale(glm::vec3(4.0f, 4.0f, 4.0f));
	app->addModel(apple);

	Model *floor = new Model(myFloorVertices, myFloorTexCoords, myFloorNormals, myFloorColors, myFloorVertexCount);
	floor->setTexture("wood.png");
	floor->rotate(glm::vec3(90.0f, 0.0f, 0.0f));
	floor->scale(glm::vec3(6.0f, 6.0f, 1.0f));
	app->addModel(floor);
	
	Model *snakeHead = new Model("cylinder.obj", "snake.png");
	snakeHead->translate(glm::vec3(0, 2, 0));
	snakeHead->rotate(glm::vec3(0,0,90));
	snakeHead->scale(glm::vec3(2.0f, 2.0f, 2.0f));
	app->addModel(snakeHead);

	Model *snake = new Model("cylinder.obj", "snake.png");
	snake->rotate(glm::vec3(0, 0, 90));
	app->addModel(snake);
	snake->scale(glm::vec3(2.0f, 2.0f, 2.0f));
	for(int i = 1; i < 5; i++) {
		app->addModel(new Model(*app->getModel(3+i-1)));
	}
	for(int i = 1; i <= 5; i++) {
		app->getModel(3+i-1)->translate(glm::vec3(5.0f, i*-0.2f, 0.0f));
	}
	std::cout << "Resources loaded... starting app loop." << std::endl;

	app->randomizeApple();

	//Main loop
	while (!glfwWindowShouldClose(app->getWindow()))
	{
		app->drawScene();
		if(app->getArbitralValue() != 0) 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		else {
			app->updateInput();
		}
	}

	delete apple;
	delete app;
}

