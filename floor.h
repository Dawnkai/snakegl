#ifndef FLOOR_H_
#define FLOOR_H_

int myFloorVertexCount=6;

float myFloorVertices[]={
    1.0f,-1.0f,-1.0f,1.0f,
    -1.0f, 1.0f,-1.0f,1.0f,
    -1.0f,-1.0f,-1.0f,1.0f,

    1.0f,-1.0f,-1.0f,1.0f,
    1.0f, 1.0f,-1.0f,1.0f,
    -1.0f, 1.0f,-1.0f,1.0f,
};

float myFloorColors[]={
    1.0f,0.0f,0.0f,1.0f,
    1.0f,0.0f,0.0f,1.0f,
    1.0f,0.0f,0.0f,1.0f,

    1.0f,0.0f,0.0f,1.0f,
    1.0f,0.0f,0.0f,1.0f,
    1.0f,0.0f,0.0f,1.0f,
};

float myFloorNormals[]={
    0.0f, 0.0f,-1.0f,0.0f,
    0.0f, 0.0f,-1.0f,0.0f,
    0.0f, 0.0f,-1.0f,0.0f,

    0.0f, 0.0f,-1.0f,0.0f,
    0.0f, 0.0f,-1.0f,0.0f,
    0.0f, 0.0f,-1.0f,0.0f,
};

float myFloorVertexNormals[]={
    1.0f,-1.0f,-1.0f,0.0f,
    -1.0f, 1.0f,-1.0f,0.0f,
    -1.0f,-1.0f,-1.0f,0.0f,

    1.0f,-1.0f,-1.0f,0.0f,
    1.0f, 1.0f,-1.0f,0.0f,
    -1.0f, 1.0f,-1.0f,0.0f,
};

float myFloorTexCoords[]={
    1.0f, 1.0f, 0.0f,0.0f, 0.0f,1.0f,
    1.0f, 1.0f, 1.0f,0.0f, 0.0f,0.0f,
};

#endif
