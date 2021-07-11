#include "libs.h"

//----------------------------------------------------------------------------------------------------------------------------------
// STANDARD FUNCTIONS

// Read texture into GPU
GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	// Load the image from filename into memory
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	if (error) std::cout << "ERROR::IMAGE::Cannot load image " << filename << " , error: " << error << std::endl;

	// Import loaded image from memory into GPU handle
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return tex;
}

//-----------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------
// MODEL CLASS


// Load vertex position from OBJ file
void Model::setPosition(std::vector<glm::vec4> *position, std::vector<GLint> *indexPosition) {
    this->vertexPosition = new float[position->size() * 4];
    this->vertexCount = position->size();

    for(int i = 0; i < position->size(); i++) {
        this->vertexPosition[i * 4] = position->at(indexPosition->at(i) - 1).x;
        this->vertexPosition[(i * 4) + 1] = position->at(indexPosition->at(i) - 1).y;
        this->vertexPosition[(i * 4) + 2] = position->at(indexPosition->at(i) - 1).z;
        this->vertexPosition[(i * 4) + 3] = position->at(indexPosition->at(i) - 1).w;
    }
}


// Load vertex texture coordinates from OBJ file
void Model::setTexCoord(std::vector<glm::vec2> *texCoord, std::vector<GLint> *indexTexCoord) {
    this->vertexTexCoord = new float[texCoord->size() * 2];

    for(int i = 0; i < texCoord->size(); i++) {
        this->vertexTexCoord[i * 2] = texCoord->at(indexTexCoord->at(i) - 1).x;
        this->vertexTexCoord[(i * 2) + 1] = texCoord->at(indexTexCoord->at(i) - 1).y;
    }
}


//Load vertex normals from OBJ file
void Model::setNormal(std::vector<glm::vec4> *normal, std::vector<GLint> *indexNormal) {
    this->vertexNormal = new float[normal->size() * 4];

    for(int i = 0; i < normal->size(); i++) {
        this->vertexNormal[i * 4] = normal->at(indexNormal->at(i) - 1).x;
        this->vertexNormal[(i * 4) + 1] = normal->at(indexNormal->at(i) - 1).y;
        this->vertexNormal[(i * 4) + 2] = normal->at(indexNormal->at(i) - 1).z;
        this->vertexNormal[(i * 4) + 3] = normal->at(indexNormal->at(i) - 1).w;
    }
}


// Fill vertex color with RGB white color <size> times
void Model::setColor(int size) {
    this->vertexColor = new float[size * 4];
    for(int i = 0; i < size; i++) {
        this->vertexColor[i * 4] = 1.0f;
        this->vertexColor[(i * 4) + 1] = 1.0f;
        this->vertexColor[(i * 4) + 2] = 1.0f;
        this->vertexColor[(i * 4) + 3] = 1.0f;
    }
}


// Load texture from file and apply to model
// Equal to readTexture function
void Model::setTexture(const char *filepath) {
	glActiveTexture(GL_TEXTURE0);

	// Load the image from filename into memory
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filepath);

	if (error) std::cout << "ERROR::IMAGE::Cannot load image " << filepath << " , error: " << error << std::endl;

	// Import loaded image from memory into GPU handle
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


// Load OBJ file and fetch object data
void Model::initalize(const char *filepath) {
    this->model.loadOBJ(filepath);
    this->model.calculateResult();
}


// Resize model instance
void Model::scale(glm::vec3 scale) {
    this->modelMatrix = glm::scale(this->modelMatrix, scale);
}


// Rotate model instance
void Model::rotate(glm::vec3 rotation) {
    this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelMatrix = glm::rotate(this->modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}


// Move model instance
void Model::translate(glm::vec3 translation) {
    this->modelMatrix = glm::translate(this->modelMatrix, translation);
}


// Activate texture of model instance
void Model::activateTexture(ShaderProgram *sp) {
    glUniform1i(sp->u("tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
}


// Send model data to GPU
void Model::sendToShader(ShaderProgram *sp) {
    // Send model matrix to GPU
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(this->modelMatrix));

    // Send object to GPU
    glVertexAttribPointer(sp->a("position"), 4, GL_FLOAT, false, 0, this->getPosition());
	glVertexAttribPointer(sp->a("texcoord"), 2, GL_FLOAT, false, 0, this->getTexCoord());
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, this->getNormal());
}

//-----------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------
// SNAKE CLASS

Snake::Snake(Model* snakeHead, Model* snake, int baseLength) {
    this->length = 1;
    this->baseLength = baseLength + 1;

    this->snakeHead = snakeHead;
    this->snakeBody = snake;

    this->snake.push_back(new Model(*this->snakeHead));

    this->extendSnake(baseLength);
}


Snake::~Snake() {

}


void Snake::extendSnake(int amount) {
    if(length <= 1) {
        this->snake.push_back(new Model(*this->snakeBody));
        amount--;
        this->length++;
    }
    for(int i = 0; i < amount; i++) {
        this->snake.push_back(new Model(*this->snake.at(this->length - 1)));
    }
    this->length += amount;
}


void Snake::renderSnake(ShaderProgram *sp) {
    for(int i = 0; i < this->snake.size(); i++) {
        this->snake.at(i)->sendToShader(sp);
        this->snake.at(i)->activateTexture(sp);

        if (this->snake.at(i)->getIndexCount() == 0) glDrawArrays(GL_TRIANGLES, 0, this->snake.at(i)->getVertexCount());
        else glDrawElements(GL_TRIANGLES, this->snake.at(i)->getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}


void Snake::move(int direction, int pos) {
    if(checkCollision()) {
        std::cout << "Snake collided with wall or himself, you died!" << std::endl;
        exit(EXIT_SUCCESS);
    }
    if(pos == 0) {
        this->snake.at(pos)->translate(glm::vec3(0.0f, this->velocity, 0.0f));
        if(direction == 2)
            this->snake.at(pos)->rotate(glm::vec3(this->angle, 0.0f, 0.0f));
        if(direction == 3)
            this->snake.at(pos)->rotate(glm::vec3(-this->angle, 0.0f, 0.0f));
        if(appleCollision()) {
            this->randomizeApple();
            this->animationIndex = 0;
        }
        return;
    }
    this->snake.at(pos)->setModelMatrix(this->snake.at(pos-1)->getModelMatrix());
    this->move(direction, pos-1);
}


void Snake::animation() {
    if(animationInbound) {
        this->snake.at(animationIndex)->scale(glm::vec3(0.8f, 0.8f, 0.8f));
        animationInbound = false;
        animationIndex++;
    }

    if(!animationInbound && animationIndex <= this->snake.size() - 1 && animationIndex >= 0) {
        this->snake.at(animationIndex)->scale(glm::vec3(1.25f, 1.25f, 1.25f));
        this->animationInbound = true;
    }

    if(animationIndex == this->snake.size() && !animationInbound) {
        this->extendSnake(1);
        this->snake.at(length - 1)->translate(glm::vec3(0.0f, -this->velocity, 0.0f));
        animationIndex = -1;
    }
}


bool Snake::checkCollision() {
    glm::mat4 head = this->snake.at(0)->getModelMatrix();
    head = glm::translate(head, glm::vec3(0.0f, this->velocity, 0.0f));
    bool wallCollision = (abs(head[3][0]) > 5.5 || abs(head[3][2]) > 5.5 );
    bool snakeCollision = false;
    for(int i = this->baseLength; i < this->snake.size(); i++)
        if(abs(head[3][0] - this->snake.at(i)->getModelMatrix()[3][0]) < 0.3 && abs(head[3][2] - this->snake.at(i)->getModelMatrix()[3][2]) < 0.3)
            snakeCollision = true;
    return wallCollision || snakeCollision; 
}


bool Snake::appleCollision() {
    glm::mat4 modelApple = this->apple->getModelMatrix();
    glm::mat4 modelSnake = this->snake.at(0)->getModelMatrix();
    return abs(modelApple[3][0] - modelSnake[3][0]) < 0.35 && abs(modelApple[3][2] - modelSnake[3][2]) < 0.45;
}


void Snake::randomizeApple() {
    glm::mat4 apple = this->apple->getModelMatrix();
    apple[3][0] = rand()%7-3.5;
    apple[3][2] = rand()%7-3.5;
    this->apple->setModelMatrix(apple);
}


void Snake::appleAnimation() {
    glm::mat4 appleMatrix = apple->getModelMatrix();

    this->apple->rotate(glm::vec3(0.0f, 2.0f, 0.0f));

    if(appleMatrix[3][1] > 1.5) {
        appleMatrix[3][1] = 1.5;
        apple->setModelMatrix(appleMatrix);
        appleTranslation = -appleTranslation;

    } else if(appleMatrix[3][1] < 1) {
        appleMatrix[3][1] = 1;
        apple->setModelMatrix(appleMatrix);
        appleTranslation = -appleTranslation;
    }

    this->apple->translate(glm::vec3(0.0f, appleTranslation, 0.0f));
}


//-----------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------
// GAME CLASS


// Initialize OpenGL program
App::App(int width, int height, const char *title) {

    // Initialize GLFW
    if (!glfwInit()) {
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

    // Create main window
    this->window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!this->window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    // Set window as main context
    glfwMakeContextCurrent(this->window);
    // Enable Vsync
	glfwSwapInterval(1);

    // Start GLEW
    GLenum err;
	if ((err = glewInit()) != GLEW_OK) {
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

    // Initialize window
    this->init();
}


// Free OpenGL resources
App::~App() {
	glfwDestroyWindow(this->window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}


// Draw objects using Lambertian shading
void App::renderLambertObjects() {
    this->lambert->use();

    this->updateUniforms(this->lambert);

    // Enable attributes for Lambert shading
    this->setAttribArrays(this->lambert);

    // Draw walls
    for(int i = 0; i < this->models.size(); i++) {
        this->models.at(i)->sendToShader(this->lambert);
        this->models.at(i)->activateTexture(this->lambert);
        glDrawArrays(GL_TRIANGLES, 0, this->models.at(i)->getVertexCount());
    }

    // Draw floor
    this->models.at(5)->sendToShader(this->lambert);
    this->models.at(5)->activateTexture(this->lambert);
    
    if (this->models.at(5)->getIndexCount() == 0) glDrawArrays(GL_TRIANGLES, 0, this->models.at(5)->getVertexCount());
    else glDrawElements(GL_TRIANGLES, this->models.at(5)->getIndexCount(), GL_UNSIGNED_INT, 0);

    // Remove attributes from GPU
    this->disableAttribArrays(this->lambert);
}


// Render objects using inverted Lambert shading
void App::renderInvertedLambertObjects() {
    this->invLambert->use();

    this->updateUniforms(this->invLambert);

    // Enable attributes for inverted Lambert shading
    this->setAttribArrays(this->invLambert);

    this->snake->appleAnimation();
    this->snake->animation();

    // Draw snake
    this->snake->renderSnake(this->lambert);

    // Draw apple
    this->models.at(0)->sendToShader(this->invLambert);
    this->models.at(0)->activateTexture(this->invLambert);

    glDrawArrays(GL_TRIANGLES, 0, this->models.at(0)->getVertexCount());

    // Remove attributes from GPU
    this->disableAttribArrays(this->lambert);
}


// Render objects using Phong shading
void App::renderPhongObjects() {
    this->phong->use();

    this->updateUniforms(this->phong);

    // Enable attributes for Phong shading
    this->setAttribArrays(this->phong);

    // Disable Phong shading
    this->disableAttribArrays(this->phong);
}


// Update camera matrices
void App::updateUniforms(ShaderProgram *sp) {
    // Calculate View Matrix based on camera position
    if(cameraToggle) this->V = this->camera.getViewMatrix();
    else this->V = glm::lookAt(glm::vec3(0.0f, 8.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.25f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Send camera to GPU
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(this->P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(this->V));

    // Change light based on camera position
    if(cameraToggle) glUniform4fv(sp->u("lightDir"), 1, glm::value_ptr(this->camera.getPosition()));
    else glUniform4fv(sp->u("lightDir"),1,glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
}


// Draw entire scene + models
void App::drawScene() {
    // Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render objects
	this->renderLambertObjects();
    this->renderInvertedLambertObjects();
    this->renderPhongObjects();

	// Swap buffers
    glfwSwapBuffers(this->window);
}


// Set options for OpenGL and create Shader Program
void App::init() {
    glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(this->window, this->windowResizeCallback);
	glfwSetKeyCallback(this->window, this->keyCallback);
    // Disable mouse
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	this->lambert = new ShaderProgram("standard_vs.glsl", NULL, "standard_fs.glsl");
    this->invLambert = new ShaderProgram("inverted_vs.glsl", NULL, "inverted_fs.glsl");
    this->phong = new ShaderProgram("phong_vs.glsl", NULL, "phong_fs.glsl");
}


// Change viewport on window resize
void App::windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    glViewport(0,0,width,height);
}


// Handle input from user
void App::keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


// Remove model from application
void App::removeModel(int pos) {
    std::vector<Model *>::iterator iter = this->models.begin() + pos;
    this->models.erase(iter);
}


// Activate attribute arrays in Shader Program
void App::setAttribArrays(ShaderProgram *sp) {
    glEnableVertexAttribArray(sp->a("position"));
	glEnableVertexAttribArray(sp->a("texcoord"));
	glEnableVertexAttribArray(sp->a("normal"));
}


// Disable attribute arrays in Shader Program
void App::disableAttribArrays(ShaderProgram *sp) {
    glDisableVertexAttribArray(sp->a("position"));
	glDisableVertexAttribArray(sp->a("texcoord"));
	glDisableVertexAttribArray(sp->a("normal"));
}


// Get recent input and act accordingly
void App::updateInput() {
    // Poll input
    glfwPollEvents();
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) this->snake->move(UP, this->snake->getLength() - 1);
    else if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) this->snake->move(DOWN, this->snake->getLength() - 1);
    else if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) this->snake->move(LEFT, this->snake->getLength() - 1);
    else if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) this->snake->move(RIGHT, this->snake->getLength() - 1);
    else if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(this->window, GL_TRUE);
    else if (glfwGetKey(this->window, GLFW_KEY_V) == GLFW_PRESS) this->toggleCamera();

    // Move camera
    else if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS) this->camera.moveCamera(this->dt, UP);
    else if (glfwGetKey(this->window, GLFW_KEY_DOWN) == GLFW_PRESS) this->camera.moveCamera(this->dt, DOWN);
    else if (glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) this->camera.moveCamera(this->dt, LEFT);
    else if (glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) this->camera.moveCamera(this->dt, RIGHT);

    // Rotate camera
    this->updateMouse();
    this->camera.rotateCamera(this->dt, this->mouseOffsetX, this->mouseOffsetY);

    // Update delta time
    this->updateDt();
}


// Update mouse parameters
void App::updateMouse() {
    glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

    if (this->firstMouse) {
        this->lastMouseX = this->mouseX;
        this->lastMouseY = this->mouseY;
        this->firstMouse = false;
    }

    this->mouseOffsetX = this->mouseX - this->lastMouseX;
    this->mouseOffsetY = this->lastMouseY - this->mouseY;

    this->lastMouseX = this->mouseX;
    this->lastMouseY = this->mouseY;
}


// Update delta time for smoother mouse movement
void App::updateDt() {
    this->curTime = static_cast<float>(glfwGetTime());
    this->dt = this->curTime - this->lastTime;
    this->lastTime = this->curTime;
}

//-----------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------
// CAMERA CLASS


// Get view matrix
const glm::mat4 Camera::getViewMatrix() {
    this->updateCamera();
    this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
    return this->viewMatrix;
}


// Update camera attributes
void Camera::updateCamera() {
    this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front.y = sin(glm::radians(this->pitch));
    this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

    this->front = glm::normalize(this->front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}


// Change camera rotation based on mouse position
void Camera::rotateCamera(const float &dt, const double &offsetX, const double &offsetY) {
    this->pitch += static_cast<GLfloat>(offsetY) * SENSITIVITY * dt;
    this->yaw += static_cast<GLfloat>(offsetX) * SENSITIVITY * dt;

    // Prevent gimbal lock
    if (this->pitch > 80.0f) this->pitch = 80.0f;
    else if (this->pitch < -80.0f) this->pitch = -80.0f;

    if (this->yaw > 360.0f || this->yaw < -360.0f) this->yaw = 0.0f;
}


// Change camera position based on keyboard input
void Camera::moveCamera(const float &dt, const int direction) {
    switch (direction) {
        case UP:
            this->position += this->front * MOVEMENT_SPEED * dt;
            break;
        
        case DOWN:
            this->position -= this->front * MOVEMENT_SPEED * dt;
            break;
        
        case LEFT:
            this->position -= this->right * MOVEMENT_SPEED * dt;
            break;
        
        case RIGHT:
            this->position += this->right * MOVEMENT_SPEED * dt;
            break;
        
        default:
            break;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------