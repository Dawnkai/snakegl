#ifndef LOADER_H
#define LOADER_H

// OpenGL libraries
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// OpenGL math libraries
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Standard C++ libraries
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdlib.h>

// Contains all data about vertices in .obj file
struct Vertex {
    float position[4];
    float texCoord[2];
    float color[4];
    float normal[4];
};


struct Face {
    int position_index;
    int texture_index;
    int normal_index;
};

class OBJModel {
    private:
        // Result variables
        // --------------------------------------------------------
        // Vertex data
        std::vector<glm::fvec4> vertex_position;
        std::vector<glm::fvec4> vertex_normal;
        std::vector<glm::fvec2> vertex_texcoord;

        // Face data
        std::vector<std::vector<Face>> faces;

        std::vector<Vertex> result;
        // --------------------------------------------------------
    
    public:
        float *vertices;
        float *normals;
        float *colors;
        float *texCoords;

        void loadOBJ(const char *filename);
        void calculateResult();
        void getPositions();
        void getNormals();
        void getTexCoords();
        void getColors();
        int getVertexCount();
        glm::fvec3 calculateNormals(int pos);
        glm::fvec3 calculatePostion(int pos);
        glm::fvec2 calculateTexCoords(int pos);
};

#endif