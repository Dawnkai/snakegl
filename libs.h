#ifndef LIBS_H_
#define LIBS_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "shaderprogram.h"
#include "lodepng.h"

extern int HEIGHT;
extern int WIDTH;

// STRUCTS
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};


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
        virtual ~Object() {}
    
        void initalize(const Vertex* vertices, const unsigned nrVertices, const GLuint* indices, const unsigned nrIndices);

        Vertex *getVertices() { return this->vertices.data(); }
        GLuint *getIndices() { return this->indices.data(); }
        const unsigned getSizeVertices() { return this->vertices.size(); }
        const unsigned getSizeIndices() { return this->indices.size(); }
};


class Floor : public Object {
    public:
        Floor() : Object() {
            Vertex verts[] = {
                // Position							// Color							// Texcoords				// Normals
                glm::vec3(-0.5f, 0.5f, 0.0f),		glm::vec3(1.0f, 0.0f, 0.0f),		glm::vec2(0.0f, 1.0f),		glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(-0.5f, -0.5f, 0.0f),		glm::vec3(0.0f, 1.0f, 0.0f),		glm::vec2(0.0f, 0.0f),		glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.5f, -0.5f, 0.0f),		glm::vec3(0.0f, 0.0f, 1.0f),		glm::vec2(1.0f, 0.0f),		glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.5f, 0.5f, 0.0f),		glm::vec3(0.0f, 1.0f, 0.0f),		glm::vec2(1.0f, 1.0f),		glm::vec3(0.0f, 0.0f, 1.0f)
            };

            unsigned int nrOfVerts = sizeof(verts) / sizeof(Vertex);

            GLuint indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            unsigned int nrOfIndic = sizeof(indices) / sizeof(GLuint);

            this->initalize(verts, nrOfVerts, indices, nrOfIndic);
        }
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

        void initMesh(Vertex *vertexData, const unsigned &nrVertices, GLuint *indexData, const unsigned &nrIndices, ShaderProgram *sp);
        void initMesh(Object *object, ShaderProgram *sp);
        void updateUniforms(ShaderProgram *sp);

    public:
        Mesh(Vertex *vertexData, const unsigned &nrVertices,
             GLuint *indexData, const unsigned &nrIndices,
             ShaderProgram *sp) {
            this->initMesh(vertexData, nrVertices, indexData, nrIndices, sp);
            this->update();
        }

        Mesh(Vertex *vertexData, const unsigned &nrVertices,
             GLuint *indexData, const unsigned &nrIndices,
             ShaderProgram *sp, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
            this->position = position;
            this->rotation = rotation;
            this->scale = scale;
            this->initMesh(vertexData, nrVertices, indexData, nrIndices, sp);
            this->update();
        }

        Mesh(Object *object, ShaderProgram *sp) {
            this->initMesh(object, sp);
            this->update();
        }

        Mesh(Object *object, ShaderProgram *sp, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
            this->position = position;
            this->rotation = rotation;
            this->scale = scale;
            this->initMesh(object, sp);
            this->update();
        }

        ~Mesh() {
            glDeleteVertexArrays(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
            glDeleteBuffers(1, &this->EBO);
        }

        void update();
        void render(ShaderProgram *sp);
        void setPosition(const glm::vec3 position);
        void setRotation(const glm::vec3 rotation);
        void setScale(const glm::vec3 scale);
};


class Game {
    private:
        GLFWwindow *window;
        ShaderProgram *sp;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        GLuint textureData[5];
        glm::vec3 lightningData[5];
        Material material0;
        glm::vec3 cameraPosition;

        std::vector<GLuint*> textures;
        std::vector<glm::vec3*> lights;
        std::vector<Material*> materials;
        std::vector<Mesh*> meshes;
        std::vector<Object*> objects;

        int width;
        int height;
        const char* title;

        void init();
        void createGame();
        void resetWindow();
        void setupScene();
        void cleanScene();
        static void error_callback(int error, const char* description);
        static void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods);
        static void windowResizeCallback(GLFWwindow* window,int width,int height);

    public:
        Game(const char *title, int width, int height);
        virtual ~Game();
        void drawScene();
        void update();

        void addTexture(GLuint *texture);
        void addLight(glm::vec3 *light);
        void addMaterial(Material *material);
        void addObject(Object *object);

        void removeTexture(int pos);
        void removeLight(int pos);
        void removeMaterial(int pos);
        void removeObject(int pos);

        GLuint *getTexture(int pos);
        glm::vec3 *getLight(int pos);
        Material *getMaterial(int pos);
        Object *getObject(int pos);

        GLFWwindow *getWindow() { return this->window; }
};

// STANDARD FUNCTIONS
GLuint readTexture(const char* filename);

#endif
