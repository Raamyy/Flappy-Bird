#include <gl/glew.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtx/transform.hpp>
#include "shader.hpp"
#include <gl/glfw3.h>
#include "FPCamera\FPCamera.h"
#include "texture.h"
#include<iostream>
#include<Windows.h>
#include<mmsystem.h>
#include "Obstacle.h"
using namespace std;
#pragma once

class Renderer
{
    GLuint VertexArrayID;
    GLuint vertexBufferID;
	GLuint myIndicesBufferID;
    GLuint programID;

	GLuint mvpMatrixID;
	glm::mat4 MVP_matrix;
	glm::mat4 MVP_matrix2;
	glm::mat4 MVP_Default;
	glm::mat4 BirdModelMatrix;
	glm::mat4 ModelMatrix;

	Texture* texture1;
	Texture* texture2;
	Texture* texture3;
	Texture* Groundtexture;
	Texture* BackGroundtexture;
	int frame;
public:
	FPCamera Camera;
    Renderer();
    ~Renderer();
    
    void Initialize();
    bool Draw();
    void Cleanup();
    void onGameEnd();
	void handleKeyboard(int);
	void handleMouse(double,double);
	bool isColided(Obstacle* o);
};
