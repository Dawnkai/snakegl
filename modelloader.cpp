// All in one model loader into OpenGL
// Maciej Kleban 147031
// Marek Pietkiewicz 147032
// Group I1.2
// PUT WIiT

#include "modelloader.h"

// Tutorial
// .obj file contains special prefixes
// v - vertices (vec3)
// vt - vertex texture coordinate (vec2)
// vn - vertex normal (vec3)
// f - faces
// Face contains numbers of vertices that fit with each other

void OBJModel::loadOBJ(char *filename) {
    std::stringstream stream;
    std::ifstream input(filename);

    std::string line;
    std::string prefix = "";

    glm::vec4 temp_vec4(1.0f);
    glm::vec2 temp_vec2;

    Face temp_face;
    std::vector<Face> temp_faces;


    if (!input.is_open()) {
        std::cout << "Cannot open file " << filename << std::endl;
        return;
    }

    while(std::getline(input, line)) {
        stream.clear();
        stream.str(line);
        stream >> prefix;

        if (prefix == "v") {
            temp_vec4.w = 1.0f;
            stream >> temp_vec4.x >> temp_vec4.y >> temp_vec4.z;
            vertex_position.push_back(temp_vec4);
        }

        else if (prefix == "vt") {
            stream >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoord.push_back(temp_vec2);
        }

        else if (prefix == "vn") {
            temp_vec4.w = 0.0f;
            stream >> temp_vec4.x >> temp_vec4.y >> temp_vec4.z;
            vertex_normal.push_back(temp_vec4);
        }

        else if (prefix == "f") {
            while (stream >> line) {
                temp_face.position_index = stoi(line.substr(0, line.find('/')));
                line.erase(0, line.find('/') + 1);
                temp_face.texture_index = stoi(line.substr(0, line.find('/')));
                line.erase(0, line.find('/') + 1);
                temp_face.normal_index = stoi(line.substr(0, line.find('/')));
                temp_faces.push_back(temp_face);
            }
            faces.push_back(temp_faces);
            temp_faces.clear();
        }
    }

    std::cout << "Model: " << filename << std::endl;
    std::cout << "Number of vertices: " << vertex_position.size() << std::endl;
    std::cout << "Number of texture coordinates: " << vertex_texcoord.size() << std::endl;
    std::cout << "Number of vertex normals: " << vertex_normal.size() << std::endl;
    std::cout << "Number of faces: " << faces.size() << std::endl;
}


glm::fvec3 OBJModel::calculateNormals(int pos) {
    if (vertex_normal.size() < pos) return glm::fvec3(0.0f);
    return vertex_normal.at(pos);
}


glm::fvec2 OBJModel::calculateTexCoords(int pos) {
    if (vertex_texcoord.size() < pos) return glm::fvec2(0.0f);
    return vertex_texcoord.at(pos);
}


glm::fvec3 OBJModel::calculatePostion(int pos) {
    if (vertex_position.size() < pos) return glm::fvec3(0.0f);
    return vertex_position.at(pos);
}


void OBJModel::calculateResult() {
    Vertex x;
    glm::fvec3 temp_vec3;
    glm::fvec2 temp_vec2;
    for(int i = 0; i < faces.size(); i++) {
        for (int j = 0; j < faces.at(i).size(); j++) {
            // Sort positions
            temp_vec3 = calculatePostion(faces.at(i).at(j).position_index - 1);
            x.position[0] = temp_vec3.x;
            x.position[1] = temp_vec3.y;
            x.position[2] = temp_vec3.z;
            x.position[3] = 1.0f;

            // Sort texture coordinates
            temp_vec2 = calculateTexCoords(faces.at(i).at(j).texture_index - 1);
            x.texCoord[0] = temp_vec2.x;
            x.texCoord[1] = temp_vec2.y;

            // Sort normals
            temp_vec3 = calculateNormals(faces.at(i).at(j).normal_index - 1);
            x.normal[0] = temp_vec3.x;
            x.normal[1] = temp_vec3.y;
            x.normal[2] = temp_vec3.z;
            x.normal[3] = 0.0f;

            // Insert green color
            x.color[0] = 0.0f;
            x.color[1] = 1.0f;
            x.color[2] = 0.0f;
            x.color[3] = 1.0f;

            // Append to result
            result.push_back(x);
        }
    }
    getNormals();
    getPositions();
    getTexCoords();
    getColors();
}

void OBJModel::getNormals() {
    normals = new float[result.size() * 4];
    int pos = 0;

    for(int i = 0; i < result.size(); i++) {
        normals[pos++] = result.at(i).normal[0];
        normals[pos++] = result.at(i).normal[1];
        normals[pos++] = result.at(i).normal[2];
        normals[pos++] = result.at(i).normal[3];
    }
}

void OBJModel::getPositions() {
    vertices = new float[result.size() * 4];
    int pos = 0;

    for(int i = 0; i < result.size(); i++) {
        vertices[pos++] = result.at(i).position[0];
        vertices[pos++] = result.at(i).position[1];
        vertices[pos++] = result.at(i).position[2];
        vertices[pos++] = result.at(i).position[3];
    }
}

void OBJModel::getTexCoords() {
    texCoords = new float[result.size() * 2];
    int pos = 0;

    for(int i = 0; i < result.size(); i++) {
        texCoords[pos++] = result.at(i).texCoord[0];
        texCoords[pos++] = result.at(i).texCoord[1];
    }
}

void OBJModel::getColors() {
    colors = new float[result.size() * 4];
    int pos = 0;

    for(int i = 0; i < result.size(); i++) {
        colors[pos++] = result.at(i).color[0];
        colors[pos++] = result.at(i).color[1];
        colors[pos++] = result.at(i).color[2];
        colors[pos++] = result.at(i).color[3];
    }
}

int OBJModel::getVertexCount() {
    return result.size();
}
