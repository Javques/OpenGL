#pragma once
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <string>
#include "tools/Shader.h"
using namespace glm;

struct cubeNode {
    mat4 ownMat;
    cubeNode* next;

};

// Set an object's material properties for lighting
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

class cubeList {


public:

    bool shuffling;
    bool jamesShuffling;
    bool moShuffling;
    bool jayShuffling;
    bool pabloShuffling;
    bool akhiShuffling;
    vec3 vecJayArray[8]{ (vec3(1, 0, 0)), (vec3(-1, 1, 0)),
            (vec3(-1, 0, 0)),
          (vec3(0, 1, 0)),
            (vec3(1, 0, 0)),
            (vec3(0, 0, 1)),
             (vec3(-1, -1, 0)),
           (vec3(0, 1, -2))
    };
    vec3 vecNewJayArray[8]{};
    vec3 vecMoArray[8]{ (vec3(-1, 0, 0))
           , (vec3(0, 1, 0))
           , (vec3(0, 0, 1))
            , (vec3(-1, 0, -1))
            , (vec3(0, 1, 0))
            , (vec3(0, 0, -1))
            , (vec3(1, -1, 1))
            , (vec3(0, 1, 0)) };
    vec3 vecNewMoArray[8]{};
    vec3 vecArray[8] = { (vec3(1.0f, 0, 0)),(vec3(-1.0f, 1.0f, 0)),(vec3(-1.0f, -1.0f, 0)),(vec3(1.0f, -1.0f, 0)),
        (vec3(0, 1.0f, 1.0f)),(vec3(1.0f, 0, 0)),(vec3(0, 1.0f, 0)), (vec3(-1.0f, 0, 0)) };
    vec3 newVecArray[8] = {};
    vec3 jamesVecArray[8] = { (vec3(1.0f, 0, 0)),(vec3(0, 1.0f, 0)),(vec3(0, 1.0f, 0)),(vec3(1.0f, 0.0f, 0)),
         (vec3(0, 0.0f, 1.0f)),(vec3(1.0f, 0, 0)),(vec3(0, -1.0f, 0)), (vec3(-1.0f, 0, 0)) };
    vec3 jamesNewVecArray[8] = {};
    vec3 cubePositionsPabloShuffled[9] = {};
    vec3 cubePositionsAkhiShuffled[9] = {};
    cubeList(bool shuffle);
    void drawGroundPlane();
    void toggleShuffle();
    void toggleShuffleJames();
    void toggleShuffleMo();
    void toggleShuffleJay();
    unsigned int createVertexNormal();
    unsigned int createLightVertexNormal();
    unsigned int createOriginalCube();
    unsigned int createOriginalWall();
    int createColorCube();
    int createTexturedCube();
    void bindTextures();
    int createPabloCube();
    int createAkhiCube();
    void alexisCube(cubeNode** root);
    void jayCube(mat4 allPos[9], mat4 originalPos);
    void updateList(cubeNode** root, mat4 newBase);
    void moWall(mat4 originalPos, mat4 wallPos[7]);
    void alexisWall(mat4 originalPos, mat4 wallPos[7]);
    void jayWall(mat4 originalPos, mat4 wallPos[7]);
    void jamesCube(cubeNode** root);
    void jamesUpdateList(cubeNode** root, mat4 newBase);
    void moCube(mat4 allPos[9], mat4 originalPos);
    void jamesWall(mat4 originalPos, mat4 wallPos[22]);
    void jamesShuffle();
    void alexisShuffle();
    void moShuffle();

    void jayShuffle();
    void drawAkhiCube(mat4 model_Matrix, Shader shaderProgram, int drawMode, std::string matName);
    void akhiToggleShuffle();
    void akhiShuffle();
    void drawPabloCube(mat4 model_Matrix, Shader shaderProgram, int drawMode, std::string matName);

    void pabloToggleShuffle();
    void pabloShuffle();
    void drawAkhiOrPabloWall(mat4 model_Matrix, Shader shaderProgram, int drawMode, std::string matName, vec3 wallPositions[], int size);
    ;





    glm::vec3 cubePositionsAkhi[9] = {

   glm::vec3(0.0f,  0.0f, 0.0f),
   glm::vec3(0.0f,-1.0f,0.0f),
   glm::vec3(0.0f,-1.0f,0.0f),

   glm::vec3(1.0f,0.0f,0.0f),
   glm::vec3(1.0f,0.0f,0.0f),
   glm::vec3(0.0f,2.0f,0.0f),

   glm::vec3(-1.0f,0.0f,0.0f),
   glm::vec3(-1.0f,0.0f,1.0f),
   glm::vec3(0.0f,0.0f,1.0f),
    };

    int sizeAkhiWall = 42;

    glm::vec3 wallPositionsAkhi[42] = {
        glm::vec3(-2.0f,2.0f,-9.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),

        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),

        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(4.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),

        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-2.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),

        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(4.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),

        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),
        glm::vec3(-1.0f,0.0f,0.0f),


        glm::vec3(0.0f,-1.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),
    };
    glm::vec3 cubePositionsPablo[9] = {
          glm::vec3(0.0f,  0.0f, 0.0f),

     glm::vec3(0.0f,  1.0f, 0.0f),
     glm::vec3(0.0f,  1.0f, 0.0f),


     glm::vec3(1.0f,  0.0f, 0.0f),
     glm::vec3(1.0f,  0.0f, 0.0f),

     glm::vec3(-3.0f,  0.0f, 0.0f),
     glm::vec3(-1.0f,  0.0f, 0.0f),

     glm::vec3(2.0f,  0.0f, 1.0f),
     glm::vec3(0.0f,  0.0f, -2.0f),
    };
    int sizePabloWall = 28;
    glm::vec3 wallPositionsPablo[28] = {

        //Bottom Layer - Layer 0
        glm::vec3(-3.0f, -1.0f, -9.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),

        //Layer 1
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-2.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),


        //Layer 2
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),

        //Layer 3
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(-6.0f, 0.0f, 0.0f),

        //Layer 4
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
    };
};