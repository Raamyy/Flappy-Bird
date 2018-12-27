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
using namespace std;
#pragma once


class Obstacle
{

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint mvpMatrixID;

	glm::mat4 ModelMatrix;
	glm::mat4 MVP;

	int id;

	GLfloat* verts;

public:
	GLfloat height; // obstacle height
	GLfloat width; // obstacle width 
	GLfloat currentX; // current position in x axis
	int position; //0->Up or 1->Down

	Obstacle(GLfloat height, GLfloat width,int pos);
	Obstacle();//
	void initialize(GLuint programID, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix);
	void Draw(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix);
	~Obstacle();
};

