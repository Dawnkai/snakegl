#ifndef LIBS_H_
#define LIBS_H_

#define MOVEMENT_SPEED 3.0f
#define SENSITIVITY 5.0f

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
#include "shaderprogram.h"
#include "lodepng.h"

extern int HEIGHT;
extern int WIDTH;

enum direction { UP = 0, DOWN, LEFT, RIGHT };

// STRUCTS
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};


// STANDARD FUNCTIONS
GLuint readTexture(const char* filename);
std::vector<Vertex> loadOBJ(const char *filename);


// CLASSES
class Material {
    private:
        glm::vec3 ambientLight;
        glm::vec3 diffuseLight;
        glm::vec3 specularLight;
        GLint diffuseTex;
        GLint specularTex;
    
    public:
        Material(glm::vec3 ambient = glm::vec3(0.1f), glm::vec3 diffuse = glm::vec3(1.0f),
                 glm::vec3 specular = glm::vec3(1.0f), GLint diffuseTex = -1,
                 GLint specularTex = -1) {
            this->ambientLight = ambient;
            this->diffuseLight = diffuse;
            this->specularLight = specular;
            if (diffuseTex != -1) this->diffuseTex = diffuseTex;
            if (specularTex != -1) this->specularTex = specularTex;
        }

        void setDiffuseTex(GLint diffuseTex);
        void setSpecularTex(GLint specularTex);
        void sendToShader(ShaderProgram *sp);
};


class Object {
    private:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

    public:
        Object() {}
        Object(const Vertex* vertices, const unsigned nrVertices, const GLuint* indices, const unsigned nrIndices) {
            this->initalize(vertices, nrVertices, indices, nrIndices);
        }
        virtual ~Object() {}
    
        void initalize(const Vertex* vertices, const unsigned nrVertices, const GLuint* indices, const unsigned nrIndices);

        Vertex *getVertices() { return this->vertices.data(); }
        GLuint *getIndices() { return this->indices.data(); }
        const unsigned getSizeVertices() { return this->vertices.size(); }
        const unsigned getSizeIndices() { return this->indices.size(); }
};


class Mesh {
    private:
        unsigned nrVertices = 0;
        unsigned nrIndices = 0;

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::mat4 ModelMatrix;

        GLuint VAO;
        GLuint VBO;
        GLuint EBO;

        void initMesh(Vertex *vertexData, const unsigned &nrVertices, GLuint *indexData, const unsigned &nrIndices);
        void initMesh(Object *object);
        void updateUniforms(ShaderProgram *sp);

    public:

        Mesh() {}

        Mesh(Object *object) {
            this->initMesh(object);
            this->update();
        }

        ~Mesh() {
            glDeleteVertexArrays(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
            if (nrIndices > 0) glDeleteBuffers(1, &this->EBO);
        }

        void update();
        void render(ShaderProgram *sp, Material material);
        void setPosition(const glm::vec3 position);
        void setRotation(const glm::vec3 rotation);
        void setScale(const glm::vec3 scale);
};


class Model {
    private:
        Material material;
        glm::mat4 modelMatrix;
        GLuint texture;
        Object *object;
        Mesh *mesh;


    public:
        Model(const char *texturePath, Object *object, glm::mat4 modelMatrix = glm::mat4(1.0f)) {
            this->object = object;
            this->init(texturePath, object, modelMatrix);
        }

        Model(const char *texturePath, const Vertex* vertices, const unsigned nrVertices,
              const GLuint* indices, const unsigned nrIndices, glm::mat4 modelMatrix = glm::mat4(1.0f)) {
            this->object = new Object(vertices, nrVertices, indices, nrIndices);
            this->init(texturePath, object, modelMatrix);
        }

        Model(const char *texturePath, const char *filepath, glm::mat4 modelMatrix = glm::mat4(1.0f)) {
            std::vector<Vertex> data = loadOBJ(filepath);
            this->object = new Object(data.data(), data.size(), NULL, 0);
            this->init(texturePath, object, modelMatrix);
        }

        void init(const char *texturePath, Object *object, glm::mat4 modelMatrix) {
            this->modelMatrix = modelMatrix;
            this->texture = readTexture(texturePath);
            this->material = Material();
            this->material.setDiffuseTex(this->texture);
            this->mesh = new Mesh(this->object);
        }

        Mesh *getMesh() { return this->mesh; }
        GLuint getTexture() { return this->texture; }
        Material getMaterial() { return this->material; }
        Object *getObject() { return this->object; }

        ~Model() {
            delete this->object;
            delete this->mesh;
        }
};


class Camera {
    private:
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;

        GLfloat pitch = 0.0f;
        GLfloat yaw = -90.0f;
        GLfloat roll = 0.0f;
        GLfloat movementSpeed = MOVEMENT_SPEED;
        GLfloat sensitivity = SENSITIVITY;

        void rotateCamera(const float &dt, const double &offsetX, const double &offsetY);
        void initCamera();

    public:
        Camera() {
            this->initCamera();
        }

        ~Camera() {}

        const glm::mat4 getViewMatrix() {
            this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
            return this->viewMatrix;
        }

        const glm::vec3 getPosition() const { return this->position; }

        void moveCamera(const float &dt, const int direction);

        void updateCamera(const float &dt, const int direction, const double &offsetX, const double &offsetY) {
            this->rotateCamera(dt, offsetX, offsetY);
            this->initCamera();
        }
};


class Game {
    private:
        GLFWwindow *window;
        ShaderProgram *sp;
        Camera camera;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        std::vector<GLuint*> textures;
        std::vector<glm::vec3*> lights;
        std::vector<Material*> materials;
        std::vector<Mesh*> meshes;
        std::vector<Model*> models;

        int width;
        int height;
        const char* title;

        // Delta time (FPS related)
        float dt = 0.0f;
        float curTime = 0.0f;
        float lastTime = 0.0f;

        // Mouse position for mouse movement
        double lastMousePosX = 0.0f;
        double lastMousePosY = 0.0f;
        double mousePosX = 0.0f;
        double mousePosY = 0.0f;
        double mouseOffsetX = 0.0f;
        double mouseOffsetY = 0.0f;
        bool firstMouse = true;

        void init();
        void createGame();
        void setupScene();
        void cleanScene();
        void updateKeyboard();
        void updateMouse();
        void tick();
        static void error_callback(int error, const char* description);
        static void windowResizeCallback(GLFWwindow* window,int width,int height);

    public:
        Game(const char *title, int width, int height);
        virtual ~Game();
        void drawScene();
        void updateInput();

        void addTexture(GLuint texture);
        void addLight(glm::vec3 *light);
        void addMaterial(Material material);
        void addModel(Model *model);

        void removeTexture(int pos);
        void removeLight(int pos);
        void removeMaterial(int pos);
        void removeModel(int pos);

        GLuint *getTexture(int pos);
        glm::vec3 *getLight(int pos);
        Material *getMaterial(int pos);
        Model *getModel(int pos);

        GLFWwindow *getWindow() { return this->window; }
};

#endif
