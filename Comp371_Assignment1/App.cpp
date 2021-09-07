
//totally based on https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
#include <iostream>
#include <random>
#include <chrono>
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include <map>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

#include <irrklang/irrKlang.h>
#include "Camera.hpp"
#include "GLShader.hpp"
#include "Scene.hpp"
#include "Model.h"
#include "Model3D.h"
#include "tools/Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "res/textures/stb_image.h"
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

using namespace glm;
using namespace irrklang;
using namespace std::chrono_literals;

static ISoundEngine* soundEngine = createIrrKlangDevice();
static irrklang::ISound* snd;

struct SpaceCube {
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 size;
    glm::vec3 vel;
    float ang;
    float angvel;
    int shaderParam;
};
std::vector<SpaceCube> cubes = {};

bool debugTick = false;
int ticks = 0;

void playbackStart();

bool playing = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void handleKeyHold(float deltaTime);
unsigned int loadTexture(const char* path);
unsigned int loadTextureSRGB(const char* path);
void draw(GLsizei size);
unsigned int loadCubemap(vector<std::string> faces);

void tRotate90(float rotEnd, float* action, vec3 axis);

// combo modifers
int renderMode = 0;
float velMod = 1;
float invMod = 1;
bool onFire = false;
float starSpeed = 1;
// bg elements
void createSpaceCube();
void updateSpaceCubes();
void drawSpaceCubes(Shader shader);

// settings
unsigned int scr_width = 1024;
unsigned int scr_height = 768;

int currentModel = 0;
int image = 0;
int screenTex = 0;
int boomBlur = 0;
     int bg= 0;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
glm::vec3 cameraPositions[6] = { glm::vec3(-50, 2, 50), glm::vec3(50, 3, 50), glm::vec3(-50, 2, -40), glm::vec3(50, 3,-40), glm::vec3(0, 5, 32), glm::vec3(0, 5, -22) };

glm::vec3 objectPosition[5] = { vec3(0, 5, 0),vec3(0, 5, 0),vec3(0, 5, 0),vec3(0, 5, 0),vec3(0, 5, 0) };

// lighting
struct PointLight pointLight;

bool bloom = true;

float exposure = 1;


// CURRENT MODEL
mat4* mat4Ptr = nullptr;
mat4* wallMat4Ptr = nullptr;
mat4* checkMat4Ptr = nullptr;
float wallDistance = 9.0f;
float theSpeedFactor = 1.0f;
bool fits;
mat4 rotateMat = mat4();
mat4 theModel = mat4(1.0f);

//Background Scene 
//instanced grass
unsigned int const amountGrass = (300*190);
glm::mat4* grassMatrices;
float grassDisplacement[amountGrass];

// TEXTURES
unsigned int transparentVAO = 0;
unsigned int transparentVBO = 0;
unsigned int backgroundPlaneVBO = 0;
unsigned int backgroundPlaneVAO = 0;
unsigned int grassTexture;
unsigned int cubeTexture;
unsigned int floorTexture;
unsigned int windowTexture;
unsigned int skyboxTexture;


unsigned int mineGrass;
unsigned int diffuseMap1;
unsigned int diffuseMap2;
unsigned int diffuseMap3;


//
//vector<std::string> faces =
//{
//    "res/textures/Meadow/negx.jpg",
//    "res/textures/Meadow/negy.jpg",
//    "res/textures/Meadow/negz.jpg",
//    "res/textures/Meadow/posx.jpg",
//    "res/textures/Meadow/posy.jpg",
//    "res/textures/Meadow/posz.jpg"
//};
//
//



void renderModel(const Shader& shader, Model3D::Model& ourModel)
{
    //Render imported 3D model
    ourModel.Draw(shader);
}

// TIMING
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float tCamera = 0;

double BPM = 60;

// INTERPOLATE VALUES FOR TIMED ACTIONS
float actionTimed[5] = { 0,0,0,0,0 };
bool actionLock[5] = { 0,0,0,0,0 };

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
unsigned int skyVAO = 0;
unsigned int skyVBO = 0;
unsigned int planeVAO = 0;
int drawMode = GL_TRIANGLES;
bool collision = false;

quat orientation;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};


void debugMetronome()
{
}

void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // positions          // normals           // texture coords
               -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
               -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

               -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
               -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

               -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
               -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
               -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
               -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

               -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
               -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

               -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
               -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
               -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(drawMode, 0, 36);
    glBindVertexArray(0);
}

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}


float skyboxVertices[] = {        
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void renderBackgroundPlane() {
    if (backgroundPlaneVAO == 0) {
        float backgroundPlaneVertices[] = {
            50.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,     50.0f, 0.0f,
           -50.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,     0.0f, 50.0f,
           -50.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,     0.0f, 0.0f,

            50.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,     50.0f, 0.0f,
            50.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,     50.0f, 50.0f,
           -50.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,     0.0f, 50.0f,
        };
        // plane VAO
     
        glGenVertexArrays(1, &backgroundPlaneVAO);
        glGenBuffers(1, &backgroundPlaneVBO);
        glBindVertexArray(backgroundPlaneVAO);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundPlaneVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundPlaneVertices), &backgroundPlaneVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(backgroundPlaneVAO);
    glDrawArrays(drawMode, 0, 6);
    glBindVertexArray(0);
}

void renderWall() {
    if (planeVAO == 0) {
        float planeVertices[] = {
            // positions            // normals         // texcoords
             50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

             50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
            -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
             50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  50.0f, 50.0f
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
    }
    glBindVertexArray(planeVAO);
    glDrawArrays(drawMode, 0, 6);
    glBindVertexArray(0);

}


void renderTransparentQuad() {
    if (transparentVAO == 0) {
        float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
          

            0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  0.0f,

            0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  1.0f,
        };
        // plane VAO

        glGenVertexArrays(1, &transparentVAO);
        glGenBuffers(1, &transparentVBO);
        glBindVertexArray(transparentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    glBindVertexArray(transparentVAO);
    glDrawArrays(drawMode, 0, 6);
    glBindVertexArray(0);
}



void renderIndexedTransparentQuad(int index) {
    if (transparentVAO == 0) {
        float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)


            0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  0.0f,

            0.0f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,    1.0f,  1.0f,
        };

        //Instance data in an array buffer


        unsigned int instanceVBO;
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, amountGrass * sizeof(glm::mat4), &grassMatrices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        unsigned int displacementVBO;
        glGenBuffers(1, &displacementVBO);
        glBindBuffer(GL_ARRAY_BUFFER, displacementVBO);
        glBufferData(GL_ARRAY_BUFFER, amountGrass * sizeof(float), &grassDisplacement[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // plane VAO

        glGenVertexArrays(1, &transparentVAO);
        glGenBuffers(1, &transparentVBO);
        glBindVertexArray(transparentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        //Set displacement data
        glBindBuffer(GL_ARRAY_BUFFER, displacementVBO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(3, 1);
       

        // also set instance data
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

       
    }
    glBindVertexArray(transparentVAO);
    glDrawArraysInstanced(drawMode, 0, 6, index);
    glBindVertexArray(0);
}



void renderBackground( Shader shader, Shader grassShader, Model3D::Model& ourModel) {
  
    float rotationalAngle = sin(glfwGetTime());
    float angle = -90.0f + (rotationalAngle * 30.0f);
    // floor
    shader.use();
    shader.setInt("diffuseTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -75.0f));
    shader.setMat4("model", model);
    renderBackgroundPlane();

    //Palm Trees
    for (float j = -100; j < -50; j += 5)
    {
        for (float i = -50; i < 50; i += 5)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i, 0.0f, j));
            model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
            model = glm::rotate(model, radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
            shader.setMat4("model", model);
            renderModel(shader, ourModel);

        }
    }

    //Grass
    //float grassAngle=(sin(glfwGetTime()) * 20.0f);
    float grassAngle = -((rotationalAngle) * 20.0f);
    grassAngle = glm::radians(grassAngle);
    grassShader.use();
    grassShader.setInt("diffuseTexture", 0);
    shader.setFloat("grassAmount", amountGrass);
    grassShader.setFloat("time", glfwGetTime());
    grassShader.setFloat("angle", grassAngle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    model = glm::mat4(1.0f);
    //float angle = -90.0f + (sin(glfwGetTime()) * 30.0f);
    model = glm::rotate(model, radians(angle), glm::vec3(0.0f,1.0f, 0.0f));
    grassShader.setMat4("model", model);
    renderIndexedTransparentQuad(amountGrass);


    //TRANSPARENCY AND ORDER
     /*1. Draw all opaque objects first
     2. Sort all the transparent objects
     3. Draw all transparent objects from furthest to nearest*/
    // cubes + windows;

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    for (float i = -50; i < -4; i += 1)
    {
        if (i == -50 || i == -5)
        {
            for (int j = 1; j < 10; j++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, j, -50.0f));
                shader.setMat4("model", model);
                renderCube();
            }

        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i, 0.0f, -50.0f));
        shader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i, 10.0f, -50.0f));
        shader.setMat4("model", model);
        renderCube();
    }

    for (float i = 5; i < 50; i += 1)
    {
        if (i == 5 || i == 49)
        {
            for (int j = 1; j < 10; j++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, j, -50.0f));
                shader.setMat4("model", model);
                renderCube();
            }

        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i, 0.0f, -50.0f));
        shader.setMat4("model", model);
        renderCube();


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i, 10.0f, -50.0f));
        shader.setMat4("model", model);
        renderCube();
    }
    
    shader.setBool("isWindow", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    for (float i = -50; i < -5; i += 1)
    {
        for (int j = 1; j < 10; j++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i, j, -50.0f));
            shader.setMat4("model", model);
            renderTransparentQuad();
        }

    }

    for (float i = 5; i < 49; i += 1)
    {
        for (int j = 1; j < 10; j++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(i, j, -50.0f));
            shader.setMat4("model", model);
            renderTransparentQuad();
        }
    }

    shader.setBool("isWindow", false);

}



// renders the 3D scene
// --------------------

int currentScore = 0; 

//HUD functions
struct Letters {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};
unsigned int letterVAO, letterVBO;
std::map<char, Letters> LetterMap;
FT_Library ft;
FT_Face face;
int Score = 0;
GLint64 timer = 120;
int wallCounter = 0;
int multiplier = 1;
glm::mat4 letterScaleMatrix = glm::mat4(1.0f);
glm::mat4 three_Dimensions = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 8.5f);


int InitFont(const char* typeFace) {
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::Freetype not initialized" << std::endl;
        return -1;
    }
    if (FT_New_Face(ft, typeFace, 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void LoadCharacters() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Letters letters = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        LetterMap.insert(std::pair<char, Letters>(c, letters));
    }
}


void freeFont() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color)
{

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // activate corresponding render state	
    s.use();
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(letterVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Letters ch = LetterMap[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, letterVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

void Render_3D_GUI(Shader& letterShader, glm::mat4 projection, glm::mat4 view) {
    // Wall Counter GUI

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(5, 6, 5));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Number", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "of Walls", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "Cleared: ", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, std::to_string(wallCounter), 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));
    //glEnable(GL_DEPTH_TEST);
    /*Reset the Matrices*/
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(5, 6, 4.75f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Number", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "of Walls", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "Cleared: ", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, std::to_string(wallCounter), 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
    //glEnable(GL_DEPTH_TEST);
    /*Reset the Matrices*/
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(5, 6, 4.5f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Number", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "of Walls", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "Cleared: ", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, std::to_string(wallCounter), 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
    //glEnable(GL_DEPTH_TEST);
    /*Reset the Matrices*/
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(5, 6, 4.25f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Number", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "of Walls", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "Cleared: ", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, std::to_string(wallCounter), 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
    //glEnable(GL_DEPTH_TEST);
    /*Reset the Matrices*/
    model = glm::mat4(1.0f);

    //Multiplier GUI
    model = glm::translate(model, glm::vec3(-4, 6, 5.f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Multiplier", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "X " + std::to_string(multiplier), 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 1.0f, 1.0f));

    /*Reset the Matrices*/
    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-4, 6, 4.75f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Multiplier", 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "X " + std::to_string(multiplier), 0.0f, 0.0f, 0.02f, glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-4, 6, 4.5));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Multiplier", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "X " + std::to_string(multiplier), 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(-4, 6, 4.25));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    letterShader.use();
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    //glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Multiplier", 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
    //model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
    letterScaleMatrix = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    three_Dimensions = projection * view * letterScaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(three_Dimensions));
    RenderText(letterShader, "X " + std::to_string(multiplier), 0.0f, 0.0f, 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));

}

void Render_EndScreen(Shader& letterShader) {
    letterShader.use();
    glm::mat4 letterProjection = glm::ortho(0.0f, static_cast<float>(scr_width), 0.0f, static_cast<float>(scr_height));
    glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(letterProjection));
    glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Game Over " , (scr_width / 2) - 120, scr_height/2, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(letterShader, "Your Score: "+std::to_string(Score), (scr_width / 2)-120, (scr_height / 2)-60, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    glEnable(GL_DEPTH_TEST);
}

void Render_2D_GUI(Shader& letterShader) {
    glDisable(GL_DEPTH_TEST);
    RenderText(letterShader, "Score: " + std::to_string(Score), 0.0f, 740.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(letterShader, "Time Left: ", scr_height + 60, 740.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    RenderText(letterShader, std::to_string((float)(timer - glfwGetTime()) / 60.0f), scr_height + 60, 700.0f, 0.8f, glm::vec3(1.0f, 1.0f, 1.0f));
    glEnable(GL_DEPTH_TEST);
}

// buttons
enum Inputs {
    RIGHT_CLICK,
    LEFT_CLICK,
    MIDDLE_CLICK,
    SHIFT,
    W,
    A,
    S,
    D,
    KEY_9,
    KEY_0
};

int inputState[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

bool isCamMoving = false;


//Alexis


cubeList cube(false);
float angles = 30.0f;
mat4 alexisBasePos = mat4(1.0f);
mat4 alexisCheckRight, jayCheckRight, jamesCheckRight,moCheckRight, akhiCheckRight;
mat4 alexisNewBase;
std::string matName = "model";
mat4 jayBasePos = mat4(1.0f);
mat4 jayNewBase;
mat4 akhiBasePos = mat4(1.0f);
mat4 akhiNewBase;
mat4 jamesBasePos = mat4(1.0f);
mat4 jamesNewBase;
mat4 moBasePos = mat4(1.0f);
mat4 moNewBase;
mat4 allWallPosJay[7];
mat4 allWallPos[7];
mat4 allJayPos[9];
mat4 allWallJames[22];
mat4 allMoPos[9];
mat4 allMoWall[7];

struct cubeNode* currentNode;
struct cubeNode* jamesCurrentNode;


unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void renderModelBag(const Shader& shader, Model3D::Model& ourModel)
{
    //Render imported 3D model
    glm::mat4 model3D = theModel;
    model3D = glm::translate(model3D, glm::vec3(9.0f, 5.0f, -4.0f)); // translate it down so it's at the center of the scene
    model3D = glm::scale(model3D, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model3D);
    ourModel.Draw(shader);
    model3D = glm::mat4(1.0f);
}

void renderScene(const Shader& shader, Model3D::Model& ourModel)
{
  
    //DRAW IMPORTED MODEL
    renderModelBag(shader, ourModel);
    shader.setInt("diffuseTexture", 0);
    mat4 model_Matrix = akhiNewBase;
    vec3 vecUsed;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mineGrass);
    shader.setInt("shininess", 64);
    shader.setVec3("specularVec", glm::vec3(0.8, 0.8, 0.8));
    shader.setFloat("isModel",false);

    if (!onFire)
    {
        // THE FLOOR
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        renderWall();
    }



    glBindTexture(GL_TEXTURE_2D, diffuseMap1);
    shader.setInt("shininess", 64);
    shader.setVec3("specularVec", glm::vec3(0.2, 0.2, 0.2));
    shader.setFloat("isModel", true);
    if (currentModel == 0) {
        //alexis model
        for (int i = 0; i < 9; i++) {

            shader.setMat4("model", (currentNode->ownMat));
            renderCube();
            currentNode = currentNode->next;
        }
    }

    else if (currentModel == 2) {
        //james 
        for (int i = 0; i < 9; i++) {
            shader.setMat4("model", (jamesCurrentNode->ownMat));
            renderCube();
            jamesCurrentNode = jamesCurrentNode->next;
        }
    }
    else if (currentModel == 3) {
        //jay draw model
        for (int i = 0; i < 9; i++) {
            shader.setMat4("model", (allJayPos[i]));
            renderCube();

        }
    }
    else if (currentModel == 4) {
        //mo draw model
        for (int i = 0; i < 9; i++) {
            shader.setMat4("model", (allMoPos[i]));
            renderCube();
        }
    }
   
    else if (currentModel == 1) {

        for (int i = 0; i < 9; i++) {

            vecUsed = cube.cubePositionsAkhi[i];


            model_Matrix = glm::translate(model_Matrix, vecUsed);
            shader.setMat4(matName.c_str(), (model_Matrix));
            renderCube();
        }
    }

    //change texture
    glBindTexture(GL_TEXTURE_2D, diffuseMap3);

    shader.setInt("shininess", 64);
    shader.setVec3("specularVec", glm::vec3(0.2, 0.2, 0.2));
    shader.setFloat("isModel", false);

    if (currentModel == 0) {
        //alexis draw wall
        for (int i = 0; i < 7; i++) {
            shader.setMat4("model", allWallPos[i]);
            renderCube();
        }
    }
    
    else if (currentModel == 2) {
        //james wall
        for (int i = 0; i < 22; i++) {
            shader.setMat4("model", allWallJames[i]);
            renderCube();
        }
    }
    else if (currentModel == 3) {
        //jay draw wall
        for (int i = 0; i < 7; i++) {
            shader.setMat4("model", allWallPosJay[i]);
            renderCube();
        }
    }
    else if (currentModel == 4) {

        //mo draw wall
        for (int i = 0; i < 7; i++) {
            shader.setMat4("model", allMoWall[i]);
            renderCube();
        }
    }

    else if (currentModel == 1) {
        model_Matrix = akhiBasePos;
        glm::mat4 view_Matrix = glm::mat4(1.0f);
        glm::mat4 projection_Matrix = glm::mat4(1.0f);
        glm::mat4 MVP_MATRIX;
        for (int i = 0; i < cube.sizeAkhiWall; i++) {
            model_Matrix = glm::translate(model_Matrix, cube.wallPositionsAkhi[i]);
            //model_Matrix = glm::rotate(model_Matrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            //view_Matrix = glm::translate(view_Matrix, glm::vec3(0.0, 0.0, -5.0f));

            MVP_MATRIX = projection_Matrix * view_Matrix * model_Matrix;
            unsigned int MVP;
            shader.setMat4("model", MVP_MATRIX);
            renderCube();
        }
    }
    
}
bool cullFace = true;
bool shadow = true;
bool texture = true;
bool firstCheck = true;
bool animationDone = false;
void resetBasePos() {
    alexisBasePos = translate(mat4(1.0f), objectPosition[0]);
    akhiBasePos = translate(mat4(1.0f), objectPosition[1]);
    jayBasePos = translate(mat4(1.0f), objectPosition[3]);
    jamesBasePos = translate(mat4(1.0f), objectPosition[2]);
    moBasePos = translate(mat4(1.0f), objectPosition[4]);
    moNewBase = moBasePos;
    jamesNewBase = jamesBasePos;
    jayNewBase = jayBasePos;
    akhiNewBase = akhiBasePos;
    alexisNewBase = alexisBasePos;
    theSpeedFactor = 1.0f*(multiplier>10?10:multiplier);
    collision = false;
    firstCheck = true;
}

void advanceWallCurrentModel(mat4* currentModel, float speedFactor) {
        *currentModel = translate(*currentModel,vec3(0,0,deltaTime*speedFactor));
        mat4 temp = *currentModel;
        if ((abs(temp[3].z - wallDistance) < 0.5)) {
            collision = true;
            theSpeedFactor = 1.f;
        }
       
}

mat4* switchCurrentWallModel(int keyPressed) {
    mat4* mat4Ptr = nullptr;
    switch (keyPressed) {
    case 0:

        mat4Ptr = &alexisBasePos;
        break;

    case 3:
        mat4Ptr = &jayBasePos;
        break;
    case 1:

        mat4Ptr = &akhiBasePos;
        break;
    case 2:

        mat4Ptr = &jamesBasePos;
        break;
    case 4:
        mat4Ptr = &moBasePos;
        break;
    }
    //add your case with your model
    return mat4Ptr;
}
mat4* switchCurrentModel(int keyPressed) {
    mat4* mat4Ptr = nullptr;
    switch (keyPressed) {
    case 0:
        mat4Ptr = &alexisNewBase;
        break;

    case 3:
        mat4Ptr = &jayNewBase;
        break;
    case 1:
        mat4Ptr = &akhiNewBase;
        break;
    case 2:
        mat4Ptr = &jamesNewBase;
        break;
    case 4:
        mat4Ptr = &moNewBase;
        break;
    }
    //add your case with your model
    return mat4Ptr;
}
mat4* switchCurrentCheckRight(int keyPressed) {
    mat4* mat4Ptr = nullptr;
    switch (keyPressed) {
    case 0:
        mat4Ptr = &alexisCheckRight;
        break;

    case 3:
        mat4Ptr = &jayCheckRight;
        break;
    case 1:
        mat4Ptr = &akhiCheckRight;
        break;
    case 2:
        mat4Ptr = &jamesCheckRight;
        break;
    case 4:
        mat4Ptr = &moCheckRight;
        break;
    }
    //add your case with your model
    return mat4Ptr;
}

mat4* getOriginBase(int model) {
    mat4* mat4Ptr = nullptr;
    switch (model) {
    case 0:
        mat4Ptr = &alexisBasePos;
        break;

    case 3:
        mat4Ptr = &jayBasePos;
        break;

    case 1:
        mat4Ptr = &akhiBasePos;
        break;
    case 2:
        mat4Ptr = &jamesBasePos;
        break;
    case 4:
        mat4Ptr = &moBasePos;
    }
    //add your case with your model
    return mat4Ptr;
}

void randomOrientation(mat4* model) {

    int factor = linearRand(0, 10);
    *model = rotate(*model, radians(90.0f*factor), vec3(1,0,0));
    *model = rotate(*model, radians(90.0f * factor), vec3(0, 1, 0));
    *model = rotate(*model, radians(90.0f * factor), vec3(0, 0, 1));

}
int main()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    playbackStart();

 

    struct cubeNode* root = new cubeNode;
    root->next = NULL;
    struct cubeNode* jamesRoot = new cubeNode;
    jamesRoot->next = NULL;
    alexisBasePos = translate(alexisBasePos, objectPosition[0]);
    akhiBasePos = translate(akhiBasePos, objectPosition[1]);
    jayBasePos = translate(jayBasePos, objectPosition[3]);
    jamesBasePos = translate(jamesBasePos, objectPosition[2]);
    moBasePos = translate(moBasePos, objectPosition[4]);

    alexisCheckRight = alexisBasePos;
    akhiCheckRight = akhiBasePos;
    jayCheckRight = jayBasePos;
    jamesCheckRight = jamesBasePos;
    moCheckRight = moBasePos;

    moNewBase = moBasePos;
    jamesNewBase = jamesBasePos;
    jayNewBase = jayBasePos;
    akhiNewBase = akhiBasePos;
    alexisNewBase = alexisBasePos;


    mat4 newBase[5] = { alexisNewBase,akhiNewBase,jamesNewBase,jayNewBase,moNewBase };

    root->ownMat = alexisNewBase;
    cube.alexisCube(&root);
    jamesRoot->ownMat = jamesNewBase;
    cube.jamesCube(&jamesRoot);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    
    // MSAA
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "OpenGL Test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // GLFW event handling setup
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // init GLEW, enable depth testing, compile and load shaders
    GLenum err = glewInit();

    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_RESCALE_NORMAL);
 
    Model3D::Model ourModel("res/models/palm/palmTree.obj");
    Shader lightingShader("phong.vert", "phong.frag");
    Shader flatShader("flat.vert", "flat.frag");
    stbi_set_flip_vertically_on_load(true);
    Model3D::Model bagModel("res/models/backpack/backpack.obj");
    stbi_set_flip_vertically_on_load(false);

    Shader depth("depth.vert", "depth.frag", "depth.gs");
    Shader screenShader("screen.vert", "screen.frag");
    Shader blurShader("blur.vert", "blur.frag");
    Shader starfieldShader("starfield.vert", "starfield.frag");
    Shader modelShader("res/shaders/model3D.vert", "res/shaders/model3D.frag");
    Shader grassShader("grass.vert", "grass.frag");
    Shader skyboxShader("skybox.vert", "skybox.frag");
  

     diffuseMap1 = loadTexture("res/textures/gold_wall.jpg");
     diffuseMap2 = loadTexture("res/textures/silver.jpg");
     diffuseMap3 = loadTexture("res/textures/Satellite.jpg");
     grassTexture = loadTexture("res/textures/grass.png");
     mineGrass = loadTexture("res/textures/grassTexture.png");

     cubeTexture = loadTexture("res/textures/container_steel.png");
     floorTexture = loadTexture("res/textures/soil.jpg");
     windowTexture = loadTexture("res/textures/blending_transparent_window.png");


     // skybox VAO
     unsigned int skyboxVAO, skyboxVBO;
     glGenVertexArrays(1, &skyboxVAO);
     glGenBuffers(1, &skyboxVBO);
     glBindVertexArray(skyboxVAO);
     glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

     //Skybox texture loading
     vector<std::string> faces
     {
        "res/textures/Space/right.png",
        "res/textures/Space/left.png",
        "res/textures/Space/top.png",
        "res/textures/Space/bottom.png",
        "res/textures/Space/front.png",
        "res/textures/Space/back.png"
     };
     //stbi_set_flip_vertically_on_load(true);
     skyboxTexture = loadCubemap(faces);
     //stbi_set_flip_vertically_on_load(false);


    // FBOs

     GLuint bgFBO;
     glGenFramebuffers(1, &bgFBO);
     glBindFramebuffer(GL_FRAMEBUFFER, bgFBO);

     GLuint bgTex;
     glGenTextures(1, &bgTex);
     glBindTexture(GL_TEXTURE_2D, bgTex);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scr_width, scr_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

     glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, bgTex, 0);
     

    vec3 lightPos = vec3(0, 30, 0);
    
    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    void tActions(float rotEnd);
    void musicTriggers(Shader shader);

    lightingShader.use();
    lightingShader.setInt("diffuseTexture", 0);
    lightingShader.setInt("depthMap", 1);
    GLuint unitLines = lines();
    GLuint xGrid = Xgrid();
    GLuint zGrid = Zgrid();
    unsigned int lightCubeVAO = cube.createOriginalWall();

    screenShader.use();
    screenShader.setInt("screenTexture", 4);
    screenShader.setInt("bloomBlur", 5);
    screenShader.setInt("background", 6);

    // Screen framebuffer
    unsigned int screenFBO;
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
    
    // Bind 2 color buffers to screenFBO
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // attach texture to framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }

    // Depth buffer and stencil buffer for screenFBO
    unsigned int screenRBO;
    glGenRenderbuffers(1, &screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, screenRBO);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping pong FBO for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0
        );
    }

    // Bright image
    blurShader.use();
    blurShader.setInt("image", 0);

    //currentMat change
    mat4Ptr = switchCurrentModel(currentModel);
    wallMat4Ptr = switchCurrentWallModel(currentModel);
    checkMat4Ptr = switchCurrentCheckRight(currentModel);
    randomOrientation(mat4Ptr);

    camera.camera_position = vec3(0.0f, 10.0f, 20.0f);
    camera.moveAndLookAt(camera.camera_position, vec3(0, 5, 0));
    //HUD

    Shader letterShader("shaders/Letter.vert", "shaders/Letter.frag");
    glm::mat4 letterProjection = mat4(1.0f);

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (InitFont("\Fonts\\arial.ttf") != -1) {
        LoadCharacters();
        freeFont();
    }
    else {
        std::cout << "FREETYPE:: Init Function failed" << std::endl;
    }

    // configure VAO/VBO for text rendering
    // -----------------------------------
    glGenVertexArrays(1, &letterVAO);
    glGenBuffers(1, &letterVBO);
    glBindVertexArray(letterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, letterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glfwSetTime(0.0);


    //Grass translations
    grassMatrices = new glm::mat4[amountGrass]; 
    glm::mat4 model = glm::mat4(1.0f);
    int index = 0;
    bool drawGrassZ = true;
    bool drawGrassX = true;
    int grassNotRendered = 0;
    for (float z = -100; z < 50; z += 0.5)
    {

        if (z == -15 || z == 15) {
            drawGrassZ = !drawGrassZ;
        }
        for (float x = -50; x < 45; x += 0.5)
        {
            if (x == -15|| x == 15)
            {
                drawGrassX = !drawGrassX;
            }

            if (drawGrassZ || drawGrassX)
            {
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 grassTranslation;
                grassTranslation.x = x;
                grassTranslation.y = -0.0002f;
                grassTranslation.z = z;

                model = glm::translate(model, grassTranslation);
                model = glm::scale(model, glm::vec3(6.0, 6.0, 6.0));

                grassMatrices[index++] = model;
            }

            else {
                index++; 
                grassNotRendered++;
            }
        }

    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //Grass color gradient
    for (float i = 0; i < amountGrass; i++) {
        grassDisplacement[int(i)] = i;
    }

  

    while (!glfwWindowShouldClose(window))
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // FILL BG FBO
        glBindFramebuffer(GL_FRAMEBUFFER, bgFBO);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, scr_width, scr_height);
        starfieldShader.use();
        starfieldShader.setVec3("u_euler", camera.getEuler());
        starfieldShader.setFloat("u_time", glfwGetTime());
        starfieldShader.setVec2("u_res", vec2(scr_width, scr_height));
        starfieldShader.setFloat("u_factor", starSpeed);
        renderQuad();

        // RENDER STAR BG
        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgTex);
        renderQuad();
        glClear(GL_DEPTH_BUFFER_BIT);

        

        letterShader.use();

        letterProjection = glm::ortho(0.0f, static_cast<float>(scr_width), 0.0f, static_cast<float>(scr_height));
        glUniformMatrix4fv(glGetUniformLocation(letterShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(letterProjection));
        // use frame time deltas rather than frames
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        handleKeyHold(deltaTime);
        advanceWallCurrentModel(wallMat4Ptr, theSpeedFactor);

        cube.alexisWall(alexisBasePos, allWallPos);
        cube.jayWall(jayBasePos, allWallPosJay);
        cube.jamesWall(jamesBasePos, allWallJames);
        cube.moWall(moBasePos, allMoWall);


        cube.jayCube(allJayPos, jayNewBase);
        cube.jamesUpdateList(&jamesRoot, jamesNewBase);

        cube.updateList(&root, alexisNewBase);
        cube.moCube(allMoPos, moNewBase);

        currentNode = root;
        jamesCurrentNode = jamesRoot;

        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------

        float near_plane = 1.0f;
        float far_plane = 105.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


        //// 1. RENDER DEPTH MAP
        //// --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depth.use();
        for (unsigned int i = 0; i < 6; ++i)
            depth.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        depth.setFloat("far_plane", far_plane);
        depth.setVec3("lightPos", lightPos);
        renderScene(depth, bagModel);

        // 2. RENDER TO SCREEN TEXTURE
        // -------------------------
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
        glViewport(0, 0, scr_width, scr_height);

        lightingShader.use();
        currentNode = root;
        jamesCurrentNode = jamesRoot;


        glm::mat4 projection = glm::perspective(glm::radians(camera.perspective_zoom), (float)scr_width / (float)scr_height, 0.1f, 1000.0f);
        glm::mat4 view = camera.view();
        glm::mat4 viewSky = glm::mat4(glm::mat3(view));

      


        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        // set lighting uniforms
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.camera_position);
        lightingShader.setInt("shadows", shadow);
        lightingShader.setBool("isTexture", texture);
        // enable/disable shadows by pressing K
        lightingShader.setFloat("far_plane", far_plane);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, diffuseMap1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, diffuseMap2);


        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, diffuseMap3);

        //glActiveTexture(GL_TEXTURE6);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
        lightingShader.setFloat("time", glfwGetTime());
        renderScene(lightingShader, bagModel);


        if (!onFire)
        {

            // TREES, GRASS 
            grassShader.use();
            grassShader.setMat4("projection", projection);
            grassShader.setMat4("view", view);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            renderBackground(lightingShader, grassShader, ourModel);
            glDisable(GL_BLEND);

            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.setInt("skybox", 0);
            skyboxShader.setMat4("projection", projection);
            skyboxShader.setMat4("view", viewSky);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }
        else
        {
            // ON FIRE

            flatShader.use();
            flatShader.setMat4("projection", projection);
            flatShader.setMat4("view", view);
            flatShader.setFloat("u_time", glfwGetTime());

            drawSpaceCubes(flatShader);
            updateSpaceCubes();

            flatShader.use();

            glBindVertexArray(xGrid);
            drawXGrid(flatShader, deltaTime * velMod);
            glBindVertexArray(0);

            glBindVertexArray(zGrid);
            drawZGrid(flatShader, 0);
            glBindVertexArray(0);
        }

        Render_2D_GUI(letterShader);
        
        Render_3D_GUI(letterShader, projection, view);
        
        //Render skybox

     
      
     

        tActions(0.15f);

        // BLUR
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 6;
        blurShader.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setInt("horizontal", horizontal);
            
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }

        // APPLY SCREEN TEXTURE TO QUAD AND RENDER TO DEFAULT FRAMEBUFFER
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        screenShader.use();
        screenShader.setInt("bloom", bloom);
        screenShader.setFloat("exposure", exposure);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();

        //collsion event
        if (collision) {

            if (firstCheck) {
                quat rot = quat_cast(*checkMat4Ptr);
                quat rot2 = quat_cast(*mat4Ptr);
                float dotProduct = dot(rot, rot2);
                fits = (abs(dotProduct - 1) < 0.001);
                //speed up and change counter
                if (fits) {
                    wallCounter++;
                    if (multiplier > 4)
                        onFire = true;
                    Score += multiplier * 5;
                    starSpeed++;
                    
                    multiplier += 1;
                    theSpeedFactor += multiplier;
                    velMod += 2;
                    createSpaceCube();

                }
                //reset to 0
                else {
                    multiplier = 1;
                    velMod = 1;
                    onFire = false;
                    starSpeed = 1;
                }

                firstCheck = false;
               

                actionTimed[4] = 0.001f;
            }                
            //reset after animation
           if (animationDone) {

                currentModel = linearRand(0, 4);
                wallMat4Ptr = switchCurrentWallModel(currentModel);
                mat4Ptr = switchCurrentModel(currentModel);
                checkMat4Ptr = switchCurrentCheckRight(currentModel);
                resetBasePos();
                randomOrientation(mat4Ptr);
                
                animationDone = false;
            }
        }

        if (timer-glfwGetTime() < 0) {
            //Render_3D_GUI(letterShader, projection, view);
            //Render_EndScreen(letterShader);
            int timerEndScreen = 0;
            glfwSetTime(0);
            while (glfwGetTime()<10) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                Render_EndScreen(letterShader);
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            glfwTerminate();
            return 0;
        }
    }
    glfwTerminate();
    return 0;
}

void changeRotationModel(mat4* originModel, vec3 rotation)
{

    *originModel = rotate(*originModel, radians(90.0f), rotation);

}

void changeTranslationModel(mat4* originModel, vec3 translation)
{
    *originModel = translate(*originModel, translation);
}

void scaleModel(mat4* originModel, bool scaleUP)
{
    float scalerFactor = 1.015f;
    vec3 scaler = vec3(scalerFactor, scalerFactor, scalerFactor);
    //scale down means having the inverse of scalerFactor

    if (!scaleUP)
        scaler = vec3(1 / scalerFactor, 1 / scalerFactor, 1 / scalerFactor);
    *originModel = scale(*originModel, scaler);

}
// Must be a world basis axis (z, or y in this case), x is in degrees, stops at 90 deg
void axisRotate(mat4* originModel, float x, vec3 axis)
{
    *originModel = rotate(*originModel, radians(x), axis);
}
//Bagging animation
void tGoToBag(float rotEnd, float* action, vec3 dir) {

    const float tmin = 0.00001f;

    float y1;
    float y2;
    float ot;

    if (*action > tmin)
    {
        
        ot = *action;
        y1 = smoothstep(0.0f, rotEnd, ot);
        *action += deltaTime;
        y2 = smoothstep(0.0f, rotEnd, *action);
        if (fits) {

            if (*action<0.05) {
                changeTranslationModel(mat4Ptr, vec3(0, 0, -1));
               
            }
            else {
                changeTranslationModel(mat4Ptr, vec3(0.4/5, (sin(*action * 10)) / 10, -1/5));
            }
            if (*action > 0.6) {
                scaleModel(mat4Ptr, false);
            }
            
        }
        else {
            changeRotationModel(mat4Ptr, vec3((sin(*action * 10)) / 10, (sin(*action * 10)) / 10, (sin(*action * 10)) / 10));
            changeTranslationModel(mat4Ptr, vec3(0, -1, 0));

        }
        
    }

    if (*action > rotEnd) {
        *action = 0.0f;
        animationDone = true;
        

    }
}

// Performs time interpolation if the timed action gets a little jump start (ie. more than tmin)
// I guess an event loop?
void tActions(float rotEnd)
{

    // Rotations
    tRotate90(0.15, &actionTimed[0], vec3(0, 1, 0));
    tRotate90(0.15, &actionTimed[1], vec3(0, -1, 0));
    tRotate90(0.15, &actionTimed[2], vec3(0, 0, 1));
    tRotate90(0.15, &actionTimed[3], vec3(0, 0, -1));
    tGoToBag(1.5,  &actionTimed[4], vec3(1, 1, 0));
}

void tRotate90(float rotEnd, float *action, vec3 axis)
{
    const float tmin = 0.00001f;

    float y1;
    float y2;
    float ot;

    if (*action > tmin)
    {
        ot = *action;
        y1 = smoothstep(0.0f, rotEnd, ot);
        *action += deltaTime;
        y2 = smoothstep(0.0f, rotEnd, *action);

        // Maybe don't use mat4ptr? Another rotation matrix THEN apply when rendering? -------

        axisRotate(mat4Ptr, (y2 - y1) * 90, axis);
    }

    if (*action > rotEnd)
        *action = 0.0f;
}

void tCameraPan(float transEnd, vec3 posEnd, vec3 lookAt)
{
    vec3 y1;
    vec3 y2;
    float ot;

    if (!isCamMoving)
    {
        ot = tCamera;
        y1 = mix(vec3(0), posEnd, ot);
        tCamera += deltaTime;
        y2 = mix(vec3(0), posEnd, tCamera);

        // Maybe don't use mat4ptr? Another rotation matrix THEN apply when rendering? -------

        camera.moveAndLookAt((y2 - y1), lookAt);
    }
    if (posEnd == y2)
        isCamMoving = false;
}

void tTranslateModel(float transEnd, vec3 posEnd, vec3 lookAt)
{
    vec3 y1;
    vec3 y2;
    float ot;

    if (!isCamMoving)
    {
        ot = tCamera;
        y1 = mix(vec3(0), posEnd, ot);
        tCamera += deltaTime;
        y2 = mix(vec3(0), posEnd, tCamera);

        // Maybe don't use mat4ptr? Another rotation matrix THEN apply when rendering? -------

        camera.moveAndLookAt((y2 - y1), lookAt);
    }
    if (posEnd == y2)
        isCamMoving = false;
}

void musicTriggers(Shader shader)
{
    // per second velocity
    //float vel = BPM / 60;

    double ang = glfwGetTime();

    // metronome
    shader.setFloat("u_time", 1 * glfwGetTime());
}



// every GLFW callback is down here
// ------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    scr_width = width;
    scr_height = height;
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        inputState[RIGHT_CLICK] = true;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        inputState[LEFT_CLICK] = true;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
       inputState[MIDDLE_CLICK] = true;
        


    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        inputState[RIGHT_CLICK] = false;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        inputState[LEFT_CLICK] = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        inputState[MIDDLE_CLICK] = false;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (inputState[LEFT_CLICK])
    {
        float xoffset = xpos - lastX;
        lastX = xpos;
        lastY = ypos;
        camera.mouseLook(xoffset, 0);
    }

    else if (inputState[MIDDLE_CLICK])
    {
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.mouseZoom(yoffset);


        //float yoffset = lastY - ypos;
        //lastX = xpos;
        //lastY = ypos;
        //camera.mouseLook(0, yoffset);
    }

    else if (inputState[RIGHT_CLICK])
    {
        //float yoffset = lastY - ypos;
        //lastX = xpos;
        //lastY = ypos;
        //camera.mouseZoom(yoffset);

        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.mouseLook(0, yoffset);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    
    // Translate camera
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        inputState[W] = true;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        inputState[S] = true;
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        inputState[A] = true;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        inputState[D] = true;
    else if (key == SHIFT && action == GLFW_PRESS)
        inputState[SHIFT] = true;

    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        inputState[W] = false;
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        inputState[S] = false;
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        inputState[A] = false;
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        inputState[D] = false;
    else if (key == SHIFT && action == GLFW_RELEASE)
        inputState[SHIFT] = false;

    //toggle cull Face
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        if (cullFace) {
            glDisable(GL_CULL_FACE);
            cullFace = !cullFace;
        }
        else {
            glEnable(GL_CULL_FACE);
            cullFace = !cullFace;
        }
    }

    //toggle shadow
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        shadow = !shadow;
    }
    //toggle texture
   


    // Reset camera
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
        camera.moveAndLookAt(vec3(0, 0, 0), vec3(0, 0, -1));

   
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {


       
        boomBlur++;
        

    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {


        bg++;

    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        
        
        theSpeedFactor *= 15;
        
    }
    // Change draw mode for objects
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        drawMode = GL_POINTS;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        drawMode = GL_LINES;
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
        drawMode = GL_TRIANGLES;

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        onFire = !onFire;



    //currentMat change

   
    if (mat4Ptr) {

        // start rotate action
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            actionTimed[0] = 0.001f;
        }

        // start rotate action
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            actionTimed[1] = 0.001f;
        }
        
        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            actionTimed[2] = 0.001f;
        }

        // start rotate action
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            actionTimed[3] = 0.001f;
        }
    }
}

void draw(GLsizei size)
{
    if (drawMode == GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, 0, size);
    else if (drawMode == GL_LINES)
        glDrawArrays(GL_LINES, 0, size);
    else if (drawMode == GL_POINTS)
        glDrawArrays(GL_POINTS, 0, size);
}

void handleKeyHold(float deltaTime) {
    if (inputState[W])
        camera.keysMove(Controls::FORWARD, deltaTime);
    if (inputState[S])
        camera.keysMove(Controls::BACKWARD, deltaTime);
    if (inputState[A])
        camera.keysMove(Controls::LEFT, deltaTime);
    if (inputState[D])
        camera.keysMove(Controls::RIGHT, deltaTime);
    if (inputState[KEY_9])
        changeTranslationModel(mat4Ptr, vec3(0, 0, 6 * deltaTime));
    if (inputState[KEY_0])
        changeTranslationModel(mat4Ptr, vec3(0, 0, -6 * deltaTime));
}
//from learn opengl
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void createSpaceCube()
{

    float f1 = (rand() % 10000) / 1000;
    float f2 = (rand() % 3000) / 300;
    float f3 = (rand() % 100) / 100;

    

    SpaceCube cube;
    cube.pos = vec3(f1 + f2, f1 + f2, 100);
    cube.size = vec3(1 + f2, 1 + f2, 1 + f2);
    cube.rot = normalize(vec3(f1 + f3, f2, f3));

    vec3 od = cube.pos - vec3(0, 0, 0);

    cube.vel = normalize(od);

    // deg
    cube.ang = 20.0f * f3;
    cube.angvel = 50.0f * 2;
    

    cubes.push_back(cube);
}

void updateSpaceCubes()
{
    for (int i = 0; i < cubes.size(); i++)
    {
        if (cubes[i].pos.z > 100)
            cubes[i].pos.z = -100;

        cubes[i].pos += cubes[i].vel * deltaTime;
        cubes[i].ang += cubes[i].angvel * deltaTime;
    }
}

void drawSpaceCubes(Shader shader)
{
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    for (int i = 0; i < cubes.size(); i++)
    {
        model = translate(model, cubes[i].pos);
        model = scale(model, cubes[i].size);
        model = rotate(model, radians(cubes[i].ang), cubes[i].rot);
        shader.setMat4("model", model);
        renderCube();
    }
}

unsigned int loadTextureSRGB(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void createScreenFBO()
{
    // Screen framebuffer
    unsigned int screenFBO;
    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    // Bind 3 color buffers to screenFBO
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // attach texture to framebuffer
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }

    // Depth buffer and stencil buffer for screenFBO
    unsigned int screenRBO;
    glGenRenderbuffers(1, &screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, screenRBO);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createBlurFBO()
{
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0
        );
    }
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void createPointShadowFBO()
{
    //const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    //unsigned int depthMapFBO;
    //glGenFramebuffers(1, &depthMapFBO);
    //// create depth cubemap texture
    //unsigned int depthCubemap;
    //glGenTextures(1, &depthCubemap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    //for (unsigned int i = 0; i < 6; ++i)
    //    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //
    //// attach depth texture as FBO's depth buffer
    //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void playbackStart()
{
    snd = soundEngine->play2D("audio/RetroSynth.mp3", true, false, true);
    snd->setVolume(0.3);
}

