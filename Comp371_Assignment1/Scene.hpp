#pragma once
#include <glm/glm.hpp>
#include <glew/glew.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include "Scene.hpp"
#include "GLShader.hpp"
#include "tools/Shader.h"

#include <iostream>

struct PointLight
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 color;
	float intensity;
};

struct DirectionalLight
{
	glm::vec3 color;
	glm::vec3 direction;
	float intensity;
};

int Xgrid();
int Zgrid();
int lines();
int ground();
int bindWhiteCube();
void drawGroundPlane(Shader shader);
void drawXGrid(Shader shader, float time);
void drawZGrid(Shader shader, float time);
void drawUnitLines(Shader shader);
void drawCube(GLuint& shader);
void drawLightCube(Shader shader, glm::vec3 position);

void sceneInitOrientation();
void rotateSceneY(float degrees);
void rotateSceneX(float degrees);