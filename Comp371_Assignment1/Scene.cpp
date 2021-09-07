#include <glm/glm.hpp>
#include <glew/glew.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Scene.hpp"
#include "GLShader.hpp"
#include <iostream>
static float worldOrientationX = 0;
static float worldOrientationY = 0;
static glm::vec3 zAxis = glm::vec3(0, 0, 1);
static glm::vec3 xAxis = glm::vec3(1, 0, 0);
static glm::mat4 model;

// someone change this to store all vertices in GPU, then reference those directly in draw calls

// hard coded geometry
static const float gridLineX[] = {
        -25.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
        25.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f
};

static const float gridLineZ[] = {
        0.0f, 0.0f, 25.0f, 0.5f, 0.5f, 0.5f,
        0.0f, 0.0f, -25.0f, 0.5f, 0.5f, 0.5f
};

static const float groundPlane[] = {
        -50.0f, -5.0f, 50.0f, 0.0f, 1.0f, 0.0f,
        50.0f, -5.0f, 50.0f, 0.0f, 1.0f, 0.0f,
        50.0f, -5.0f, -50.0f, 0.0f, 1.0f, 0.0f,

        -50.0f, -5.0f, 50.0f, 0.0f, 1.0f, 0.0f,
        50.0f, -5.0f, -50.0f, 0.0f, 1.0f, 0.0f,
        -50, -5.0f, -50.0f, 0.0f, 1.0f, 0.0f

};

static const float unitLine[] = {
        0.0f, 0.0f, 0.0f, 100.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 100.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 100.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 100.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 100.0f
};

static const float whiteCube[] = {
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,  1.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f,  1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f, 1.0f,
};

static const glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  10.0f,  0.0f),
        glm::vec3(-50.0f,  0.0f, -50.0f),
        glm::vec3(50.0f,  0.0f, -50.0f),
        glm::vec3(50.0f,  0.0f, 50.0f),
        glm::vec3(-50.0f, -0.4f, 50.0f)
};

glm::mat4 rotateWorld(glm::mat4& model)
{
    model = glm::rotate(model, glm::radians(worldOrientationX), zAxis);
    return glm::rotate(model, glm::radians(worldOrientationY), xAxis);
}

//// do a dfs traversal of a modeltree, apply transform while recursing
//void drawCubeTree(CubeTree* CubeTree)
//{
//    if (cubeTree == NULL) return;
//
//    if (cubeTree->child != NULL) drawObject(cubeTree->child);
//}

void drawWithGLMode(int drawMode, int size)
{
    if (drawMode == GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, 0, size);
    else if (drawMode == GL_LINES)
        glDrawArrays(GL_LINES, 0, size);
    else if (drawMode == GL_POINTS)
        glDrawArrays(GL_POINTS, 0, size);
}

//// draws complex objects from each of the scene's modeltrees
//void drawSceneObjects()
//{
//    for (CubeTree object : objects)
//        drawObject(&object);
//}

// scene draw functions
// -----------------------------------------------------------------------------
// repeadetdly reset buffers at the moment, change this later (very bad performance)
int Xgrid() {
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridLineX), gridLineX, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;

}
int Zgrid() {
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridLineZ), gridLineZ, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;

}
int lines() {
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitLine), unitLine, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;
}

int bindWhiteCube() {
    GLuint vertexArrayObject, vertexBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(whiteCube), whiteCube, GL_STATIC_DRAW);

    glBindVertexArray(vertexArrayObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBindVertexArray(0);
    return vertexArrayObject;
}

int ground() {
    // meshes
    unsigned int planeVAO;
    // set up vertex data (and buffer(s)) and configure vertex attributes
     // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    return planeVAO;
}

float xtrans = 0;
void drawXGrid(Shader shader, float time)
{
    std::string matName = "model";
    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
    (xtrans+time < 1) ? xtrans += time : xtrans = 0;
    for (int i = -12; i < 12; i++) {
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1,1,1));
        model = glm::translate(model, glm::vec3(0, -1, i + xtrans + 0.5));
        shader.setMat4(matName.c_str(), model);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void drawZGrid(Shader shader, float time)
{
    std::string matName = "model";


    for (int i = -12; i < 12; i++) {
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(i-0.5, -1, 0));
        shader.setMat4(matName.c_str(), model);
        glDrawArrays(GL_LINES, 0, 2);
    }
}

void drawUnitLines(Shader shader)
{
    std::string matName = "model";

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5, 5, 5));
    for (int i = 0; i < 3; i++) {

        shader.setMat4(matName.c_str(), model);
        glDrawArrays(GL_LINES, 0, 6);
    }
}

void drawGroundPlane(Shader shader)
{
    std::string matName = "model";

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4(matName.c_str(), model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawLightCube(Shader shader, glm::vec3 position)
{
    std::string matName = "model";

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4(matName.c_str(), model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// Update a collection of cubes with given velocities and rotation axes
void cubesFX(std::vector<glm::mat4> cubes, std::vector<glm::vec3> vels, std::vector<glm::vec3> axes)
{

}

// functions that change the scene
void rotateSceneX(float degrees)
{
    worldOrientationX += degrees;
}

void rotateSceneY(float degrees)
{
    worldOrientationY += degrees;
}

void sceneInitOrientation()
{
    worldOrientationY = 0, worldOrientationX = 0;
}

// on god this is going to be for a later assignment...
// useful for shaders
// -----------------------------------------------------------------------------