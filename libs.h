#ifndef LIBS_H_
#define LIBS_H_

#define PI 3.14f

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "lodepng.h"
#include "shaderprogram.h"
#include "objloader.h"

enum DIRECTION { UP = 0, DOWN, LEFT, RIGHT };

GLuint readTexture(const char* filename);

class Model {
    private:
        OBJModel model;
        float *vertexPosition = NULL;
        float *vertexTexCoord = NULL;
        float *vertexNormal = NULL;
        float *vertexColor = NULL;
        GLuint *indices = NULL;
        unsigned int indexCount = 0;
        unsigned int vertexCount = 0;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        GLuint texture;

        void initalize(const char *filepath);

    public:
        Model(const char *filepath) {
            this->initalize(filepath);
        }

        Model(const char *filepath, const char *texturePath) {
            this->initalize(filepath);
            this->setTexture(texturePath);
        }

        Model(float *vertexPosition, float *vertexTexCoord, float *vertexNormal, float *colors, unsigned int vertexCount) {
            this->vertexPosition = vertexPosition;
            this->vertexTexCoord = vertexTexCoord;
            this->vertexNormal = vertexNormal;
            this->vertexColor = colors;
            this->vertexCount = vertexCount;
        }

        Model(float *vertexPosition, float *vertexTexCoord, float *vertexNormal, float *colors, GLuint *indices,
              unsigned int vertexCount, unsigned int indexCount) {
            this->vertexPosition = vertexPosition;
            this->vertexTexCoord = vertexTexCoord;
            this->vertexNormal = vertexNormal;
            this->vertexColor = colors;
            this->vertexCount = vertexCount;
            this->indices = indices;
            this->indexCount = indexCount;
        }

        void setPosition(std::vector<glm::vec4> *position, std::vector<GLint> *indexPosition);
        void setTexCoord(std::vector<glm::vec2> *texCoord, std::vector<GLint> *indexTexCoord);
        void setNormal(std::vector<glm::vec4> *normal, std::vector<GLint> *indexNormal);
        void setColor(int size);
        void setModelMatrix(glm::mat4 modelMatrix) { this->modelMatrix = modelMatrix; }
        void setTexture(GLuint texture) { this->texture = texture; }
        void setTexture(const char *filepath);

        void scale(glm::vec3 scale);
        void rotate(glm::vec3 rotation);
        void translate(glm::vec3 translation);

        float *getPosition() { return (this->vertexPosition ? this->vertexPosition : this->model.vertices); }
        float *getTexCoord() { return (this->vertexTexCoord ? this->vertexTexCoord : this->model.texCoords); }
        float *getNormal() { return (this->vertexNormal ? this->vertexNormal : this->model.normals); }
        float *getColor() { return (this->vertexColor ? this->vertexColor : this->model.colors); }
        GLuint *getIndices() { return this->indices; }

        glm::mat4 getModelMatrix() { return this->modelMatrix; }
        GLuint getTexture() { return this->texture; }

        unsigned int getVertexCount() { return (this->vertexCount != 0 ? this->vertexCount : this->model.getVertexCount()); }
        unsigned int getIndexCount() { return this->indexCount; }

        void activateTexture(ShaderProgram *sp);
        void sendToShader(ShaderProgram *sp);

        ~Model() {
            if (this->vertexPosition) delete this->vertexPosition;
            if (this->vertexNormal) delete this->vertexNormal;
            if (this->vertexColor) delete this->vertexColor;
            if (this->vertexTexCoord) delete this->vertexTexCoord;
        }
};


class App {
    private:
        GLFWwindow *window;
        const char *title;
        std::vector<Model*> models;
        ShaderProgram *sp;

        // View Matrix
        glm::mat4 V = glm::lookAt(
            glm::vec3(0.0f,0.0f,-4.0f),
            glm::vec3(0.0f,0.0f,0.0f),
            glm::vec3(0.0f,1.0f,0.0f)
        );

        // Perspective matrix
        glm::mat4 P = glm::perspective(50.0f*PI/180.0f,
            1.0f, 1.0f, 50.0f
        );

        static void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
        static void windowResizeCallback(GLFWwindow* window,int width,int height);
        void renderObjects();
        void init();
        void setAttribArrays();
        void disableAttribArrays();
        void move(int direction);
    
    public:
        App(int width, int height, const char *title);
        ~App();

        GLFWwindow *getWindow() { return this->window; }

        Model *getModel(int pos) { return this->models.at(pos); }
        void addModel(Model *model) { this->models.push_back(model); }
        void removeModel(int pos);

        void setCamera(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
            this->V = viewMatrix;
            this->P = projectionMatrix;
        }

        void drawScene();
        void updateInput();
};

#endif