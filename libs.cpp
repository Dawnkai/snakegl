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
	if ((err=glewInit()) != GLEW_OK) {
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


// Draw all objects in models attribute vector
void App::renderObjects() {
    for (int i = 0; i < this->models.size(); i++) {
        // Send object to shader program
        this->models.at(i)->sendToShader(this->sp);

        // Apply textures
        this->models.at(i)->activateTexture(this->sp);

        // Draw object
        if (this->models.at(i)->getIndexCount() == 0) glDrawArrays(GL_TRIANGLES, 0, this->models.at(i)->getVertexCount());
        else glDrawElements(GL_TRIANGLES, this->models.at(i)->getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}


// Draw entire scene + models
void App::drawScene() {
    // Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shading program
    this->sp->use();

    // Send camera to GPU
    glUniformMatrix4fv(this->sp->u("P"),1,false,glm::value_ptr(this->P));
    glUniformMatrix4fv(this->sp->u("V"),1,false,glm::value_ptr(this->V));

	// Enable attributes
    this->setAttribArrays();

	// Render objects
	this->renderObjects();

	// Remove attributes from GPU
    this->disableAttribArrays();

	// Swap buffers
    glfwSwapBuffers(this->window);
}


// Set options for OpenGL and create Shader Program
void App::init() {
    glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(this->window, this->windowResizeCallback);
	glfwSetKeyCallback(this->window, this->keyCallback);
	this->sp=new ShaderProgram("vertex_shader.glsl",NULL,"fragment_shader.glsl");
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
void App::setAttribArrays() {
    glEnableVertexAttribArray(this->sp->a("position"));
	glEnableVertexAttribArray(this->sp->a("texcoord"));
	glEnableVertexAttribArray(this->sp->a("normal"));
}


// Disable attribute arrays in Shader Program
void App::disableAttribArrays() {
    glDisableVertexAttribArray(this->sp->a("position"));
	glDisableVertexAttribArray(this->sp->a("texcoord"));
	glDisableVertexAttribArray(this->sp->a("normal"));
}


// Get recent input and act accordingly
void App::updateInput() {
    glfwPollEvents();
    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) move(UP);
    else if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) move(DOWN);
    else if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) move(LEFT);
    else if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) move(RIGHT);
    else if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(this->window, GL_TRUE);
}


// Placeholder function for moving the snake
void App::move(int direction) {

}


//-----------------------------------------------------------------------------------------------------------------------------------
