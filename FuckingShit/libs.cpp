#include "libs.h"

#define FOV 90.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define CAMERA_SPEED 0.05f

int WIDTH = 0;
int HEIGHT = 0;

// MATERIAL CLASS

// Set diffuse texture
void Material::setDiffuseTex(GLint diffuseTex) {
    this->diffuseTex = diffuseTex;
}

// Set specualr texture
void Material::setSpecularTex(GLint specularTex) {
    this->specularTex = specularTex;
}

// Send material to shader
void Material::sendToShader(ShaderProgram *sp) {
    glUniform3fv(sp->u("material.ambientLight"), 1, glm::value_ptr(this->ambientLight));
    glUniform3fv(sp->u("material.diffuseLight"), 1, glm::value_ptr(this->diffuseLight));
    glUniform3fv(sp->u("material.specularLight"), 1, glm::value_ptr(this->specularLight));
    if (diffuseTex != -1) glUniform1i(sp->u("material.diffuseTex"), this->diffuseTex);
    if (specularTex != -1) glUniform1i(sp->u("material.specularTex"), this->specularTex);
}
//----------------------------------------------------------------------------------------------------------------------------------


// MESH CLASS

// Create mesh from object instance
void Mesh::initMesh(Object *object) {
	this->nrIndices = object->getSizeIndices();
	this->nrVertices = object->getSizeVertices();

	glCreateVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->nrVertices * sizeof(Vertex), object->getVertices(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrIndices * sizeof(GLuint), object->getIndices(), GL_STATIC_DRAW);
}

// Update uniform values in shader program before rendering
void Mesh::updateUniforms(ShaderProgram *sp) {
    glUniformMatrix4fv(sp->u("modelMatrix"), 1, GL_FALSE, glm::value_ptr(this->ModelMatrix));
}

// Render object on screen through shader program
void Mesh::render(ShaderProgram *sp, Material material) {
	material.sendToShader(sp);

	glVertexAttribPointer(sp->a("position"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(sp->a("color"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(sp->a("texcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

    this->updateUniforms(sp);
    sp->use();
    if (this->nrIndices == 0) glDrawArrays(GL_TRIANGLES, 0, this->nrVertices);
    else glDrawElements(GL_TRIANGLES, this->nrIndices, GL_UNSIGNED_INT, 0);
}

// Update attributes of mesh instance
void Mesh::update() {
    this->ModelMatrix = glm::mat4(1.0f);
    this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}

// Set position of an object
void Mesh::setPosition(const glm::vec3 position) {
    this->position = position;
}

// Set rotation of an object
void Mesh::setRotation(const glm::vec3 rotation) {
    this->rotation = rotation;
}

// Set scale of an object
void Mesh::setScale(const glm::vec3 scale) {
    this->scale = scale;
}

//------------------------------------------------------------------------------------------------------------------------


// OBJECT CLASS

// Add data describing object
void Object::initalize(const Vertex* vertices, const unsigned nrVertices, const GLuint* indices, const unsigned nrIndices) {
    for (int i = 0; i < nrVertices; i++) {
        this->vertices.push_back(vertices[i]);
    }
    for (int i = 0; i < nrIndices; i++) {
        this->indices.push_back(indices[i]);
    }
}

//--------------------------------------------------------------------------------------------------------------------------


// GAME CLASS

Game::Game(const char *title, int width, int height) {
    this->title = title;
    this->width = width;
    this->height = height;
    this->cameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
    this->init();
}

Game::~Game() {
    delete this->sp;
	glfwDestroyWindow(this->window);
}

// Error handler
void Game::error_callback(int error, const char* description) {
	fputs(description, stderr);
}

// Window resizing callback
void Game::windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
	// Get new screen size and width to scale objects
	glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
}

// Initialize Game class
void Game::init() {
    GLenum err;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		std::cout << "ERROR:GLFW::Can't initialize GLFW." << std::endl;
		exit(EXIT_FAILURE);
	}

	this->window = glfwCreateWindow(this->width, this->height, this->title, NULL, NULL);

	if (!this->window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(this->window); 
	// Enable VSync
	glfwSwapInterval(1);

	// Start GLEW
	if ((err=glewInit()) != GLEW_OK) {
		std::cout << "ERROR::GLEW::" << glewGetErrorString(err) << std::endl;
		exit(EXIT_FAILURE);
	}

	// Disable cursor
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Start game
	this->createGame();
}

// Create entire game enivornment
void Game::createGame() {
    // Set clear color to black
	glClearColor(0, 0, 0, 1);

	// Enable Z algorithm
	glEnable(GL_DEPTH_TEST);

	// Enable callbacks
	glfwSetWindowSizeCallback(this->window, Game::windowResizeCallback);

	// Load textures and shaders
	this->sp=new ShaderProgram("vertex_shader.glsl", NULL, "fragment_shader.glsl");

	// Get screen size
	glfwGetFramebufferSize(this->window, &WIDTH, &HEIGHT);
}

// Setup before rendering objects
void Game::setupScene() {

	// Set view and projection matrix
	this->viewMatrix = glm::lookAt(
		this->cameraPosition,
		this->cameraPosition - glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	this->projectionMatrix = glm::perspective(
		glm::radians(FOV),
		static_cast<float>(WIDTH) / HEIGHT,
		NEAR_PLANE,
		FAR_PLANE
	);

	glUniformMatrix4fv(this->sp->u("viewMatrix"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix));
	glUniformMatrix4fv(this->sp->u("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix));

	// Enable object attributes
	glEnableVertexAttribArray(this->sp->a("position"));
	glEnableVertexAttribArray(this->sp->a("color"));
	glEnableVertexAttribArray(this->sp->a("texcoord"));
	glEnableVertexAttribArray(this->sp->a("normal"));

	// Enable lightning and camera
	glUniform3fv(this->sp->u("lightPos0"), 1, glm::value_ptr(*(this->lights[0])));
	glUniform3fv(this->sp->u("cameraPosition"), 1, glm::value_ptr(this->cameraPosition));
}

// Pool all inputs
void Game::updateInput() {
	this->updateKeyboard();
	this->updateMouse();
	this->camera.updateCamera(this->dt, -1, this->mouseOffsetX, this->mouseOffsetY);
}

// Pool keybaord input
void Game::updateKeyboard() {
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) this->cameraPosition.z -= CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) this->cameraPosition.z += CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) this->cameraPosition.x -= CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) this->cameraPosition.x += CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_Z) == GLFW_PRESS) this->cameraPosition.y -= CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_C) == GLFW_PRESS) this->cameraPosition.y += CAMERA_SPEED;
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(this->window, GL_TRUE);
}

// Pool mouse input
void Game::updateMouse() {
	glfwGetCursorPos(this->window, &this->mousePosX, &this->mousePosY);

	if (this->firstMouse) {
		this->lastMousePosX = this->mousePosX;
		this->lastMousePosY = this->mousePosY;
		this->firstMouse = false;
	}

	this->mouseOffsetX = this->mousePosX - this->lastMousePosX;
	this->mouseOffsetY = this->lastMousePosY - this->mousePosY;

	this->lastMousePosX = this->mousePosX;
	this->lastMousePosY = this->mousePosY;
}

// Update delta time for smooth fps
// Get time between frames
void Game::tick() {
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

// Teardown after rendering objects
void Game::cleanScene() {
	glDisableVertexAttribArray(this->sp->a("vertex"));
	glDisableVertexAttribArray(this->sp->a("color")); 
	glDisableVertexAttribArray(this->sp->a("texcoord"));
	glDisableVertexAttribArray(this->sp->a("normal")); 
}

// Drawing function
void Game::drawScene() {
	// Update delta time
	this->tick();

	// Clear previous buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
    this->sp->use();

	// Setup the scene
	this->setupScene();

	// Add textures to objects
	glUniform1i(this->sp->u("texture0"), 0);

	// Draw objects
	for(int i = 0; i < this->models.size(); i++) {
		this->models.at(i)->getMesh()->render(this->sp, this->models.at(i)->getMaterial());
	}

	// Clean the scene
	this->cleanScene();

	// Swap buffers
    glfwSwapBuffers(this->window);
}


// Getters
GLuint *Game::getTexture(int pos) {
    return this->textures.at(pos);
}

glm::vec3 *Game::getLight(int pos) {
    return this->lights.at(pos);
}

Material *Game::getMaterial(int pos) {
    return this->materials.at(pos);
}

Model *Game::getModel(int pos) {
	return this->models.at(pos);
}


// Setters
void Game::addTexture(GLuint texture) {
    this->textures.push_back(&texture);
}

void Game::addLight(glm::vec3 *light) {
    this->lights.push_back(light);
}

void Game::addMaterial(Material material) {
    this->materials.push_back(&material);
}

void Game::addModel(Model *model) {
	this->addTexture(model->getTexture());
	this->addMaterial(model->getMaterial());
	this->models.push_back(model);
}


// Removers
void Game::removeTexture(int pos) {
    std::vector<GLuint *>::iterator iter = this->textures.begin() + pos;
    this->textures.erase(iter);
}

void Game::removeLight(int pos) {
    std::vector<glm::vec3 *>::iterator iter = this->lights.begin() + pos;
    this->lights.erase(iter);
}

void Game::removeMaterial(int pos) {
    std::vector<Material *>::iterator iter = this->materials.begin() + pos;
    this->materials.erase(iter);
}

void Game::removeModel(int pos) {
	std::vector<Model *>::iterator iter = this->models.begin() + pos;
    this->models.erase(iter);
}
//-------------------------------------------------------------------------------------------------------------------------


// CAMERA CLASS
void Camera::rotateCamera(const float &dt, const double &offsetX, const double &offsetY) {
	this->pitch += static_cast<GLfloat>(offsetY) * this->sensitivity * dt;
	this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * dt;

	// Restrict camera rotation
	if (this->pitch > 80.0f) this->pitch = 80.0f;
	else if (this->pitch < -80.0f) this->pitch = -80.0f;

	if (this->yaw > 360.0f || this->yaw < -360.0f) this->yaw = 0.0f;
}


void Camera::moveCamera(const float &dt, const int direction) {
	switch(direction) {
		case 0:
			this->position += this->front * this->movementSpeed * dt;
			break;
		case 1:
			this->position -= this->front * this->movementSpeed * dt;
			break;
		case 2:
			this->position -= this->right * this->movementSpeed * dt;
			break;
		case 3:
			this->position += this->right * this->movementSpeed * dt;
			break;
		default:
			break;
	}
}


void Camera::initCamera() {
	this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front.y = sin(glm::radians(this->pitch));
	this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->front = glm::normalize(this->front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}
//-------------------------------------------------------------------------------------------------------------------------


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
