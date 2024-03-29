#ifndef LIBS_H_
#define LIBS_H_

#define PI 3.14f
#define NEAR_PLANE 1.0f
#define FAR_PLANE 50.0f
#define FOV 90.0f
#define ASPECT_RATIO 1.0f
#define MOVEMENT_SPEED 2.50f
#define SENSITIVITY 10.0f

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
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

class Camera {
    private:
        glm::mat4 viewMatrix = glm::mat4(1.0f);

        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 position = glm::vec3(0.0f, 8.0f, 0.0f);
        glm::vec3 front;
        glm::vec3 right = glm::vec3(0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

        GLfloat pitch = 0.0f;
        GLfloat yaw = -90.0f;
        GLfloat roll = 0.0f;

        void updateCamera();

    public:
        Camera() { this->updateCamera(); };
        ~Camera() {}

        const glm::mat4 getViewMatrix();
        const glm::vec4 getPosition() { return glm::vec4(this->position, 0.0f); }
        void rotateCamera(const float &dt, const double &offsetX, const double &offsetY);
        void moveCamera(const float &dt, const int direction);
};

class Snake {
    private:
        int length;
        int baseLength;
        int animationIndex = -1;
        float velocity = 0.2f;
        float angle = 22.0f;
        float appleTranslation = 0.001f;
        glm::vec4 applePos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        std::vector<Model*> snake;

        bool animationInbound = false;

        Model* snakeHead;
        Model* snakeBody;
        Model* apple;

        bool appleCollision();
        bool checkCollision();

    public:
        Snake(Model* snakeHead, Model* snake, int baseLength);
        ~Snake();

        void extendSnake(int amount);
        void renderSnake(ShaderProgram *sp);
        void move(int direction, int pos);
        void randomizeApple();
        void animation();
        void appleAnimation();

        void addApple(Model* apple) { this->apple = apple; }

        bool getAnimationStatus() { return this->animationInbound; }
        int getAnimationIndex() { return this->animationIndex; }
        int getLength() { return this->length; }
        glm::vec4 getApplePos() { return this->applePos; }
        
};

class App {
    private:
        // App window attributes
        GLFWwindow *window;
        const char *title;

        // All models
        std::vector<Model*> models;

        // Snake
        Snake *snake;

        // Camera
        Camera camera;

        // Lambertian shading
        ShaderProgram *lambert;
        // Inverted lambertain shading
        ShaderProgram *invLambert;
        // Phong shading
        ShaderProgram *phong;

        // View Matrix attributes
        glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -4.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 4.0f);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 V;

        // Perspective matrix
        glm::mat4 P = glm::perspective(glm::radians(FOV),
            ASPECT_RATIO, NEAR_PLANE, FAR_PLANE
        );

        int arbitralAnimationValue = 0;
        bool scaled = false;

        // Delta time (for smoother movement)
        float dt = 0.0f;
        float curTime = 0.0f;
        float lastTime = 0.0f;

        // Mouse input attributes
        double lastMouseX = 0.0f;
        double lastMouseY = 0.0f;
        double mouseX = 0.0f;
        double mouseY = 0.0f;
        double mouseOffsetX = 0.0f;
        double mouseOffsetY = 0.0f;
        bool firstMouse = true;

        bool cameraToggle = false;

        void updateDt();
        static void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
        static void windowResizeCallback(GLFWwindow* window,int width,int height);
        void renderLambertObjects();
        void renderInvertedLambertObjects();
        void renderPhongObjects();
        void init();
        void setAttribArrays(ShaderProgram *sp);
        void disableAttribArrays(ShaderProgram *sp);
        void updateUniforms(ShaderProgram *sp);
        void toggleCamera() { cameraToggle = !cameraToggle; };

    public:
        App(int width, int height, const char *title);
        ~App();

        GLFWwindow *getWindow() { return this->window; }

        Model *getModel(int pos) { return this->models.at(pos); }
        void addModel(Model *model) { this->models.push_back(model); }
        void removeModel(int pos);
        
        void addSnake(Snake *snake) { this->snake = snake; }

        int getAmountOfModels() { return this->models.size(); }
 
        void setCamera(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
            this->V = viewMatrix;
            this->P = projectionMatrix;
        }

        void drawScene();
        void updateInput();
        void updateMouse();
};



#endif