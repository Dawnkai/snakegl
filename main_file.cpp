/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "modelloader.h"
#include <random>
#include <iostream>

float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float aspectRatio=1;

// Seed random number generation
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distr(-2.0f, 2.0f);

// TODO: Move this into main function and execute
// only when apple has been eaten
float move_x = distr(gen);
float move_y = distr(gen);

ShaderProgram *sp; //Pointer to the shader program

GLuint tex0;
GLuint tex1;
GLuint tex2;
OBJModel apple;
OBJModel snakeHead;
OBJModel snakeBody;

float verts[] = {
	2.0f, -2.0f, 0.0f, 1.0f,
	-2.0f, 2.0f, 0.0f, 1.0f,
	-2.0f, -2.0f, 0.0f, 1.0f,

	2.0f, -2.0f, 0.0f, 1.0f,
	2.0f, 2.0f, 0.0f, 1.0f,
	-2.0f, 2.0f, 0.0f, 1.0f
};

float texCoords[] = {
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

// Read texture into GPU
GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	// Load the image from filename into memory
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);

	if (error) {
		std::cout << "Cannot load image " << filename << " , error: " << error << std::endl;
		exit(EXIT_FAILURE);
	}

	// Import loaded image from memory into GPU handle
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}


// Error handle
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}


// Allow resizing of main window
void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}


// Initalization
void initOpenGLProgram(GLFWwindow* window) {
	// Set clear color to black'
	glClearColor(0,0,0,1);

	// Enable Z algorithm
	glEnable(GL_DEPTH_TEST);

	// Enable callbacks
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	// Load textures and shaders
	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	tex0=readTexture("wood.png");
	tex1=readTexture("apple.png");
	tex2=readTexture("snake.png");

	apple.loadOBJ("apple.obj");
	snakeHead.loadOBJ("snake_head.obj");
	snakeBody.loadOBJ("cylinder.obj");

	apple.calculateResult();
	// snakeHead.calculateResult();
	snakeBody.calculateResult();
}


// Exit function
void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;
}


// Drawing function
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {

	// Clear previous buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute view matrix
	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,8.0f,3.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f));

	// Compute projection matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, 1.0f, 1.0f, 50.0f);

	// Activate shader program
    sp->use();

	// Add projection and view matrices to shader program
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));


	// Base position matrix setup
    glm::mat4 M=glm::mat4(1.0f);

	// Enable textures
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(sp->u("textureMap0"), 0);
	glUniform1i(sp->u("textureMap1"), 0);

	// Enable attributes
	glEnableVertexAttribArray(sp->a("vertex"));
	glEnableVertexAttribArray(sp->a("color"));
	glEnableVertexAttribArray(sp->a("normal"));
	glEnableVertexAttribArray(sp->a("texCoord0"));

	// Global attribute values
	// Green color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, apple.colors);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, apple.normals);

	// Draw floor
	glBindTexture(GL_TEXTURE_2D, tex0);
	M=glm::rotate(M, 1.6f, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verts);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Cancel rotation made by drawing floor
	M=glm::rotate(M, -1.6f, glm::vec3(1.0f, 0.0f, 0.0f));

	// Draw snake head
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, snakeHead.vertices);
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, snakeHead.colors);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, snakeHead.texCoords);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glDrawArrays(GL_TRIANGLES, 0, snakeHead.getVertexCount());

	// Draw apple
	glBindTexture(GL_TEXTURE_2D, tex1);
	// Randomize position
	M=glm::translate(M, glm::vec3(move_x, 0.0f, 0.0f));
	M=glm::translate(M, glm::vec3(0.0f, 0.0f, move_y));
	// TODO: Apple shouldnt be scaled, it breaks the texture
	M=glm::scale(M, glm::vec3(7.0f, 7.0f, 7.0f));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, apple.vertices);
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, apple.colors);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, apple.normals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, apple.texCoords);
    glDrawArrays(GL_TRIANGLES, 0, apple.getVertexCount());

	// Clear attributes
    glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color")); 
	glDisableVertexAttribArray(sp->a("normal")); 
	glDisableVertexAttribArray(sp->a("texCoord0")); 

    glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure


	float angle_x=0; //current rotation angle of the object, x axis
	float angle_y=0; //current rotation angle of the object, y axis
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
        angle_x+=speed_x*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y+=speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
        glfwSetTime(0); //Zero the timer
		drawScene(window,angle_x,angle_y); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
