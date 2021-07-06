#ifndef FLOOR_H_
#define FLOOR_H_

#include "libs.h"

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

#endif
