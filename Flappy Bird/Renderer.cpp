#include "Renderer.h"
#include "Obstacle.h"
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include<irrKlang.h>

#define RED 1.0f, 0.0f, 0.0f
#define GREEN 0.0f, 1.0f, 0.0f
#define BLUE 0.4f, 0.8f, 0.8f
#define GRAY 0.6f, 0.6f, 0.6f
#define NO_TEXTURE 404.0f,404.0f

string highScoreFile = "High Score.txt";
int highScorce = 0;

bool one = true; // For texture swapping
bool gameON = true;

float BirdPositionY = 0; // Current bird position on Y axis
GLfloat GravityForce = 0.09f; // downward Force applied on bird every frame
GLfloat JumpStep = 2.0f; // upwards force applied on bird when key pressed

int startIndex = 0; // start index for rendering pipes
int pipeCounter = 0; // Number of Spwaned Pipes


const int pipesNumber = 200; // Maximum Number of Pipes
vector<Obstacle*>pipes(pipesNumber); // Array of pipes
int Count = 0;
int PipesFrequency = 150; // every x frames a pipe spawns
int currentScore = 0;

int lastIndex = -1; //helper for sound playing

#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

ISoundEngine *SoundEngine;

static const GLfloat verts[] = {
	//BIRD
	-1.0f,-1.0f,0.0f,GREEN,
	0.0f,0.0f,

	-1.0f,1.0f,0.0f,BLUE,
	0.0f,1.0f,

	1.5f,-1.0f,0.0f,RED,
	1.0f,0.0f,

	1.5f,1.0f,0.0f,RED,
	1.0f,1.0f,

	//GROUND
	-1.5,-7.0,0.0f,RED,
	0.0f,0.0f,

	-1.5,-5.0,0.0f,RED,
	0.0f,1.0f,

	35.0f,-7.0,0.0f,RED,
	1.0f,0.0f,

	35.0f,-5.0+7.0,0.0f,RED,
	1.0f,1.0f,


};


Renderer::Renderer()
{
	frame = 0;
}

Renderer::~Renderer()
{
	Cleanup();
}


int Renderer::getHighScore(string filename)
{
	fstream file;
	file.open(filename);
	if (!file) {
		// create scores file if it doesnt exist
		ofstream newFile(filename);
		newFile << 0;
		newFile.close();
		return 0;
	}
	int score;
	file >> score;
	file.close();
	return score;
}

void Renderer::setHighScore(string filename,int newScore)
{
	fstream file;
	file.open(filename,ios::out|ios::trunc);
	if (!file) {
		cout << "Unable to open file datafile.txt";
		return;
	}
	file << newScore;
	file.close();
	return;
}

void Renderer::Initialize()
{
	srand(time(NULL));

	SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("sound effects/Theme Song.mp3", GL_TRUE);

	texture1 = new Texture("textures/bird1.png", 0);
	texture2 = new Texture("textures/bird2.png", 0);
	texture3 = new Texture("textures/bird3.png", 0);
	Groundtexture = new Texture("textures/ground.png", 0);

	highScorce = getHighScore(highScoreFile);

	// Creating Vertex-Array-Object (VAO). Not used here, check the handout to see its importance.
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	///////////////////////////////////////////////////////////////////////////////////////////
	/// Start Drawing your primitive
	////////////////////////////////

	// Set the background color to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	///							Bird       Ground
	GLushort indices[] = { 0,1,2,3,2,1 ,4,5,6,5,7,6  };


	// 3) reserve/generate a buffer object name(ID).
	// void glGenBuffers(GLsizei n, GLuint * buffers);
	// n: number of names to be generated. (you can generate more than one name)
	// buffers: names generated.
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &myIndicesBufferID);

	// 4) set vertexbufferID as the current GL_ARRAY_BUFFER.
	// note that since this is the first time we bind myBufferID,
	// in this step OpenGL will both allocate and bind the buffer object.
	// void glBindBuffer(GLenum target,GLuint buffer);
	// target: Specifies the target to which the buffer object is bound.
	// buffer: Specifies the name of a buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndicesBufferID);

	// 5) allocate the mem in the GPU and copy the data from the RAM to the GPU,
	// in the current GL_ARRAY_BUFFER, which in our case is myBufferID.
	// void glBufferData(GLenum target,GLsizeiptr size,
	// const GLvoid * data,GLenum usage);
	// target:	Specifies the target buffer object.
	// size:	Specifies the size in bytes of the buffer object's new data store.
	// data:	Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
	// usage:	Specifies the expected usage pattern of the data store.
	//	Usage can make performance difference, because it affects the place where the data is allocated,
	//	but since we won't change the data so we can make it GL_STATIC_DRAW.
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	// Use our shader
	glUseProgram(programID);

	mvpMatrixID = glGetUniformLocation(programID, "MVP");

	

	Camera.SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	Camera.Reset(
		glm::vec3(8, 0, -20), //pos camer 
		glm::vec3(8, 0, 0), //basa feen
		glm::vec3(0, -1, 0) //rotation
	);

	BirdModelMatrix = glm::mat4(1);

	ModelMatrix = glm::mat4(1);
	ModelMatrix *= glm::translate(-2.0f, 14.0f, 0.0f); //Ground

	MVP_matrix = Camera.GetProjectionMatrix()* Camera.GetViewMatrix() * BirdModelMatrix; // mvp bird
	MVP_matrix2 = Camera.GetProjectionMatrix()* Camera.GetViewMatrix() * ModelMatrix; // mvp ground

	//Intializing Pipes Array
	for (int i = 0; i < pipesNumber; i++)
	{
		//int pos =1;
		int pos = rand() % 2; // Generate random 0 or 1 for position
		int r = (rand() % 10) + 2; // Generate random number for pipe height
		pipes[i] = new Obstacle(r, 5.0f, pos);
		pipes[i]->initialize(programID, Camera.GetViewMatrix(), Camera.GetProjectionMatrix());
	}

}

bool Renderer::Draw()
{
	frame++;

	if (frame % 1000 == 0) {
		PipesFrequency -= 10;
		cout << "level up !\n";
		cout<<PipesFrequency<<'\n';
	}

	glBindVertexArray(VertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	BirdModelMatrix *= glm::translate(0.0f, GravityForce, 0.0f); // Apply Gravity Force
	BirdPositionY -= GravityForce;


	//cout << BirdPositionY << endl;
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(BLUE, 1.0f);

	//6) enable the desired attributes. (Please go to this section for more information about vertex attributes:
	// the attributes are 0 indexed, and here we have only one attribute.
	// void glEnableVertexAttribArray( GLuint index);
	// 1st attribute buffer : vertex position, later we'll add vertex color and other attributes
	//7) specify the vertex-data format.
	//	void glVertexAttribPointer(
	//	GLuint index,	(the index of the attribute you are describing)
	//	GLint size,	(the number of elements in that attribute)
	// GLenum type,	(the type of each element in that attribute)
	// GLboolean normalized, (do you want to normalize the data?)
	// GLsizei stride,	(the offset between each instance of that attribute)
	// const GLvoid * pointer (the offset of the first component of the first instance of the attribute) );

	glEnableVertexAttribArray(0); //Position Attribute
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 8,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1); // Color Attribute
	glVertexAttribPointer(
		1,							// attribute number.                   
		3,                  		// size
		GL_FLOAT,           		// type
		GL_FALSE,           		// normalized?
		sizeof(float) * 8,  		// stride
		(char*)(sizeof(float) * 3) 	// array buffer offset
	);

	glEnableVertexAttribArray(2); // Texture Coordinate
	glVertexAttribPointer(
		2,							 // attribute number. 
		2,							 // size
		GL_FLOAT,					 // type
		GL_FALSE,					 // normalized?
		sizeof(float) * 8,           // stride
		(char*)(sizeof(float) * 6)   // array buffer offset
	);

	// Update Matricees
	MVP_matrix = Camera.GetProjectionMatrix()* Camera.GetViewMatrix() * BirdModelMatrix; 
	MVP_matrix2 = Camera.GetProjectionMatrix()* Camera.GetViewMatrix() * ModelMatrix;

	// Every 10 frames change textures
	if (frame % 10 == 0 || frame == 0)
	{
		if (one)
			one = false;
		else 
			one = true;
	}


	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &MVP_matrix[0][0]);

	
	if (one)
		texture2->Bind();
	else
		texture1->Bind();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glUseProgram(programID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0); // Bird

	Groundtexture->Bind();
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &MVP_matrix2[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * 6)); // Ground


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Increment pipeCounter until count reaches the PipesFrequency
	if (Count == 0)
	{
		pipeCounter++;
	}
	Count++;
	if (Count == PipesFrequency)
		Count = 0;

	// Draw the Pipes
	
	for (int i = startIndex; i < pipeCounter; i++) {
		pipes[i]->Draw(Camera.GetViewMatrix(), Camera.GetProjectionMatrix());
		//cout << pipes[startIndex]->currentX << endl;
		if (pipes[startIndex]->currentX <= 6.0f && startIndex != lastIndex)
		{
			lastIndex = startIndex;
			currentScore++;
			SoundEngine->play2D("sound effects/coin.wav");

		}
		if (pipes[startIndex]->currentX <= 1.0f)
		{
			startIndex = i + 1;
		}


	}


	// Collision Detection with floor/celling
	if (BirdPositionY < -6.0f || BirdPositionY > 7.3f) 
	{
		onGameEnd();
		return false;
	}

	// Collision Detection with pipes
	if (isColided(pipes[startIndex]))
	{
		onGameEnd();
		return false;
	}
	return true;
	
}

void Renderer::Cleanup()
{
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteVertexArrays(1, &VertexArrayID); //the one we didn't used (will be discussed later)
	glDeleteProgram(programID);
}

void Renderer::handleKeyboard(int key)
{
	if ((key == GLFW_KEY_UP || key == GLFW_KEY_SPACE) && gameON)
	{
		BirdModelMatrix *= glm::translate(0.0f, -JumpStep, 0.0f);
		BirdPositionY += JumpStep;
		SoundEngine->play2D("sound effects/sfx_wing.wav");
		///PlaySound(TEXT("sound effects/sfx_wing.wav"), NULL, SND_FILENAME | SND_ASYNC);
		//Camera.Fly(0.1);
	}
	if (key == GLFW_KEY_DOWN)
		BirdModelMatrix *= glm::translate(0.0f, 0.0f, 0.0f);
	if (key == GLFW_KEY_LEFT)
		Camera.Strafe(-0.1f);
	if (key == GLFW_KEY_RIGHT)
		Camera.Strafe(0.1f);

	Camera.UpdateViewMatrix();
}

void Renderer::handleMouse(double mouseX, double mouseY)
{
	cout << mouseX << " " << mouseY << "\n";
}

bool Renderer:: isColided(Obstacle* o)
{

	bool collisionX = o->currentX <= 14 && o->currentX >= 6;

	bool collisionY;

	// Collision y-axis?
	if (o->position == 0) //up
		collisionY = BirdPositionY >= (8 - o->height);
	else if (o->position == 1) //down
		collisionY = BirdPositionY <= (o->height - 6.4f);


	// Collision only if on both axes
	return collisionX && collisionY;
}

void Renderer::onGameEnd() {
	gameON = false;
	
	if (currentScore > highScorce)
	{
		SoundEngine->play2D("sound effects/applause.wav");
		///PlaySound(TEXT("sound effects/applause.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cout << "CONGRATULATIONS !! you set a new High Score !\nYour Score is " << currentScore;
		setHighScore(highScoreFile, currentScore);
	}
	else {
		SoundEngine->play2D("sound effects/sfx_die.wav");
	///PlaySound(TEXT("sound effects/sfx_die.wav"), NULL, SND_FILENAME | SND_ASYNC);
		cout << "GAME OVER ! :(\n";
		cout << "Your Score is : " << currentScore;
		cout << "\nThe High Score is: " << highScorce;
	}
}