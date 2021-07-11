#define GLM_FORCE_RADIANS

#include "libs.h"
#include "floor.h"

int main(void)
{
	App *app = new App(500, 500, "Snake");

	Model *apple = new Model("apple.obj", "apple.png");
	apple->translate(glm::vec3(0.0f, 1.0f, 0.0f));
	apple->scale(glm::vec3(6.0f, 6.0f, 6.0f));
	app->addModel(apple);

	Model *floor = new Model(myFloorVertices, myFloorTexCoords, myFloorNormals, myFloorColors, myFloorVertexCount);
	Model **walls = new Model*[4];
	float translation = 6.0f;
	for(int i = 0; i < 4; i++) {
		walls[i] = new Model(*floor);
		walls[i]->setTexture("steel.png");
		walls[i]->translate(glm::vec3(0.0f, 0.5f, 0.0f));
		if(i > 1) {
			walls[i]->translate(glm::vec3(translation, 0.0f, 0.0f));
			walls[i]->rotate(glm::vec3(90.0f, 0.0f, 90.0f));
			translation = -translation;
		} else {
			walls[i]->translate(glm::vec3(0.0f, 0.0f, translation));
			walls[i]->rotate(glm::vec3(90.0f, 0.0f, 0.0f));
			translation = -translation;
		}
		walls[i]->scale(glm::vec3(6.5f, 0.5f, 1.0f));
		app->addModel(walls[i]);
	}

	floor->setTexture("wood.png");
	floor->rotate(glm::vec3(90.0f, 0.0f, 0.0f));
	floor->scale(glm::vec3(6.0f, 6.0f, 1.0f));
	app->addModel(floor);
	
	Model *snakeHead = new Model("cylinder.obj", "snake.png");
	snakeHead->translate(glm::vec3(0, 2, 0));
	snakeHead->rotate(glm::vec3(0,0,90));
	snakeHead->scale(glm::vec3(2.0f, 2.0f, 2.0f));

	Model *snakeBody = new Model("cylinder.obj", "snake.png");
	snakeBody->rotate(glm::vec3(0, 0, 90));
	snakeBody->scale(glm::vec3(2.0f, 2.0f, 2.0f));
	
	Snake *snake = new Snake(snakeHead, snakeBody, 5);
	snake->addApple(apple);
	app->addSnake(snake);

	std::cout << "Resources loaded... starting app loop." << std::endl;

	snake->randomizeApple();

	//Main loop
	while (!glfwWindowShouldClose(app->getWindow()))
	{
		app->drawScene();
		if(snake->getAnimationIndex() != -1) 
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		else {
			app->updateInput();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	//idk jak masz memo clearing, wiec usuwam jedynie dynam array, czy to dobrze, who knows
	delete walls;
	delete apple;
	delete app;
}

