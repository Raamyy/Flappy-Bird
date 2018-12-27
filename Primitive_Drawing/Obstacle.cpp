#include "Obstacle.h"
#define RED 1.0f, 0.0f, 0.0f
#define GREEN 0.45f, 0.75f, 0.18f
#define DarkerGREEN 0.45f, 0.55f, 0.18f
#define BLUE 0.4f, 0.8f, 0.8f
#define GRAY 0.6f, 0.6f, 0.6f
#define NO_TEXTURE 404.0f,404.0f





Obstacle::Obstacle(GLfloat height, GLfloat width,int pos)
{
	ModelMatrix = glm::mat4(1);
	MVP = glm::mat4(1);
	currentX = 31.5;
	position = pos; // up or down
	this->height = height;
	this->width = width;
}

Obstacle::Obstacle()
{
	
	ModelMatrix = glm::mat4(1);
	MVP = glm::mat4(1);
	currentX = 31.5;
	position = 1;
	this->height = 5.0f;
	this->width = 2.0f;
}

void Obstacle::initialize(GLuint programID, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix)
{
	
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	int size  ;
	//cout << "init "<<id << endl;

	//TOP OBSTACLE
	if (position == 0) {
		GLfloat myVerts[] = {
		19, (-8.2 + height), 0.0f, GREEN,
		NO_TEXTURE,
		
		19, -8.2,0.0f,GREEN,
		NO_TEXTURE,

		19+width,(-8.2 + height),0.0f,GREEN,
		NO_TEXTURE,

		19+width,-8.2,0.0f,GREEN,
		NO_TEXTURE,
//		------------------------------------
		19, (-8.2 + height), 1.0f,GREEN,
		NO_TEXTURE,

		19, -8.2,1.0f,GREEN,
		NO_TEXTURE,

		19 + width,(-8.2 + height),1.0f,GREEN,
		NO_TEXTURE,

		19 + width,-8.2,1.0f,GREEN,
		NO_TEXTURE,
//		--------------------------------------
		19, (-8.2 + height), 0.0f, GREEN,
		NO_TEXTURE,

		19, -8.2,0.0f,GREEN,
		NO_TEXTURE,

		19, (-8.2 + height), 1.0f, GREEN,
		NO_TEXTURE,

		19, -8.2,1.0f,GREEN,
		NO_TEXTURE,
//		--------------------------------------
		19 + width,(-8.2 + height),1.0f,GREEN,
		NO_TEXTURE,

		19 + width,-8.2,1.0f,GREEN,
		NO_TEXTURE,

		19 + width,(-8.2 + height),0.0f,GREEN,
		NO_TEXTURE,

		19 + width,-8.2,0.0f,GREEN,
		NO_TEXTURE,
		};

		size = sizeof(myVerts);
		verts = myVerts;
	}
	else {
		GLfloat myVerts[] = {
			19, 7.2, 0.0f, GREEN,
			NO_TEXTURE,

			19, (7.2-height),0.0f,GREEN,
			NO_TEXTURE,

			19 + width,7.2,0.0f,GREEN,
			NO_TEXTURE,

			19 + width,(7.2-height),0.0f,GREEN,
			NO_TEXTURE,
			//		------------------------------------
			19, 7.2, 1.0f, GREEN,
			NO_TEXTURE,

			19, (7.2 - height),1.0f,GREEN,
			NO_TEXTURE,

			19 + width,7.2,1.0f,GREEN,
			NO_TEXTURE,

			19 + width,(7.2 - height),1.0f,GREEN,
			NO_TEXTURE,
			//		--------------------------------------
			19, 7.2, 1.0f, GREEN,
			NO_TEXTURE,

			19, (7.2 - height),1.0f,GREEN,
			NO_TEXTURE,

			19, 7.2, 0.0f, GREEN,
			NO_TEXTURE,

			19, (7.2 - height),0.0f,GREEN,
			NO_TEXTURE,
			//		--------------------------------------
			19 + width,7.2,0.0f,GREEN,
			NO_TEXTURE,

			19 + width,(7.2 - height),0.0f,GREEN,
			NO_TEXTURE,

			19 + width,7.2,1.0f,GREEN,
			NO_TEXTURE,

			19 + width,(7.2 - height),1.0f,GREEN,
			NO_TEXTURE,
		};

		size = sizeof(myVerts);
		verts = myVerts;
	}
	
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW);

	glUseProgram(programID);
	mvpMatrixID = glGetUniformLocation(programID, "MVP");

	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

}

void Obstacle::Draw(glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix)
{
	//cout << "draw " << id << endl;
	glBindVertexArray(vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); 
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verts)*32, verts, GL_STATIC_DRAW);
	ModelMatrix *= glm::translate(-0.1f, 0.0f, 0.0f);
		
	currentX -= 0.1;
	//cout << currentX << endl;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 8,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 8,                  // stride
		(char*)(sizeof(float) * 3)            // array buffer offset
	);


	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 8,                  // stride
		(char*)(sizeof(float) * 6)            // array buffer offset
	);

	MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 1, 3);
	glDrawArrays(GL_TRIANGLES, 4, 3);
	glDrawArrays(GL_TRIANGLES, 5, 3);
	glDrawArrays(GL_TRIANGLES, 8, 3);
	glDrawArrays(GL_TRIANGLES, 9, 3);
	glDrawArrays(GL_TRIANGLES, 12, 3);
	glDrawArrays(GL_TRIANGLES, 13, 3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}




Obstacle::~Obstacle()
{
}
