#include "libs.h"

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

// Create mesh from raw data or object instance
void Mesh::initMesh(Vertex *vertexData, const unsigned &nrVertices, GLuint *indexData, const unsigned &nrIndices, ShaderProgram *sp) {
    this->nrIndices = nrIndices;
    this->nrVertices = nrVertices;

	glCreateVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->nrVertices * sizeof(Vertex), vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->nrIndices * sizeof(GLuint), indexData, GL_STATIC_DRAW);

    glVertexAttribPointer(sp->a("position"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(sp->a("color"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glVertexAttribPointer(sp->a("texcoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glVertexAttribPointer(sp->a("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
}

void Mesh::initMesh(Object *object, ShaderProgram *sp) {
    this->initMesh(object->getVertices(), object->getSizeVertices(), object->getIndices(), object->getSizeIndices(), sp);
}

// Update uniform values in shader program before rendering
void Mesh::updateUniforms(ShaderProgram *sp) {
    glUniformMatrix4fv(sp->u("modelMatrix"), 1, GL_FALSE, glm::value_ptr(this->ModelMatrix));
}

// Render object on screen through shader program
void Mesh::render(ShaderProgram *sp) {
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
    this->cameraPosition = glm::vec3(0.0f, 0.0f, -1.0f);
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

// Key input handler
void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// Window resizing callback
void Game::windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
	// Get new screen size and width to scale objects
	glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
}

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

	// Start game
	this->createGame();
}

void Game::createGame() {
    // Set clear color to black
	glClearColor(0,0,0,1);

	// Enable Z algorithm
	glEnable(GL_DEPTH_TEST);

	// Enable callbacks
	glfwSetWindowSizeCallback(this->window, Game::windowResizeCallback);
	glfwSetKeyCallback(this->window, Game::keyCallback);

	// Load textures and shaders
	this->sp=new ShaderProgram("vertex_shader.glsl",NULL,"fragment_shader.glsl");
	this->lightningData[0] = glm::vec3(0.0f, 0.0f, 2.0f);

	// Set view matrix
	this->viewMatrix = glm::lookAt(
        glm::vec3(0.0f,0.0f,1.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)
	);

	// Get screen size and set projection matrix
	glfwGetFramebufferSize(this->window, &WIDTH, &HEIGHT);
	this->projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
}

void Game::setupScene() {
	glUniformMatrix4fv(this->sp->u("viewMatrix"), 1, GL_FALSE, glm::value_ptr(this->viewMatrix));
	glUniformMatrix4fv(this->sp->u("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(this->projectionMatrix));

	glEnableVertexAttribArray(this->sp->a("position"));
	glEnableVertexAttribArray(this->sp->a("color"));
	glEnableVertexAttribArray(this->sp->a("texcoord"));
	glEnableVertexAttribArray(this->sp->a("normal"));

	glUniform3fv(this->sp->u("lightPos0"), 1, glm::value_ptr(this->lightningData[0]));
	glUniform3fv(this->sp->u("cameraPosition"), 1, glm::value_ptr(this->cameraPosition));
}

void Game::cleanScene() {
	glDisableVertexAttribArray(this->sp->a("vertex"));
	glDisableVertexAttribArray(this->sp->a("color")); 
	glDisableVertexAttribArray(this->sp->a("texcoord"));
	glDisableVertexAttribArray(this->sp->a("normal")); 
}

// Drawing function
void Game::drawScene() {

	// Clear previous buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
    this->sp->use();

	Floor floor;

	Mesh test(&floor, this->sp);

	// Setup the scene
	this->setupScene();

	// Add textures to objects
	glUniform1i(this->sp->u("texture0"), 0);

	this->material0.sendToShader(this->sp);

	// Draw objects
	test.render(this->sp);

	// Clean the scene
	this->cleanScene();

	// Swap buffers
    glfwSwapBuffers(this->window);
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

void Game::addTexture(GLuint *texture) {
    this->textures.push_back(texture);
}

void Game::addLight(glm::vec3 *light) {
    this->lights.push_back(light);
}

void Game::addMaterial(Material *material) {
    this->materials.push_back(material);
}

void Game::addObject(Object *object) {
    this->objects.push_back(object);
}

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

void Game::removeObject(int pos) {
    std::vector<Object *>::iterator iter = this->objects.begin() + pos;
    this->objects.erase(iter);
}

GLuint *Game::getTexture(int pos) {
    return this->textures.at(pos);
}

glm::vec3 *Game::getLight(int pos) {
    return this->lights.at(pos);
}

Material *Game::getMaterial(int pos) {
    return this->materials.at(pos);
}

Object *Game::getObject(int pos) {
    return this->objects.at(pos);
}

