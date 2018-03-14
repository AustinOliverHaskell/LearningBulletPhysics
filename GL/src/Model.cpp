#include "./h/FileLoader.h"
#include "./h/shapeData.h"
#include "./h/Controls.h"
#include "./h/Model.h"

#include <cstdlib>

using namespace glm;

Model::Model(Model &m)
{
	shapeData = m.getShapeData();
	colorData = m.getColorData();
	faceCount = m.getFaceCount();
}

Model::Model(std::string path, GLuint shade, bool affectedByPhysics,  bool tessalate)
{
	FileLoader * file = new FileLoader();

	if (!file->openFile(path, tessalate))
	{
		std::cout << "Filed to load file: " << path << std::endl;
	}

	faceCount = file->getFaceCount();

	shapeData = file->getObjectData();

	normalData = file->getNormals();

	shader = shade;

	GLfloat * colors = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		colors[i  ] = 0.5;
		colors[i+1] = 0.5;
		colors[i+2] = 0.5;
	}

	colorData = colors;

	transform = mat4(1);

	// Deaults to origin
	btDefaultMotionState* motionState = new btDefaultMotionState();
	// Assume Cube shape for collision
	collisionShape = new btBoxShape(btVector3(1, 1, 1));
	mass = 0;
	btVector3 fallInertia(0, 0, 0);
	collisionShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, fallInertia);
    rigidBody = new btRigidBody(fallRigidBodyCI);
}

Model::~Model()
{
	if (setupComplete)
	{
		glDeleteBuffers(1, &verticies);
		glDeleteBuffers(1, &colors);
	}

	delete collisionShape;
	delete motionState;
	delete rigidBody;
}

GLfloat * Model::getShapeData()
{
	return shapeData;
}

GLfloat * Model::getColorData()
{
	return colorData;
}



void Model::setColor(float r, float g, float b)
{
	GLfloat * color = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		color[i  ] = r;
		color[i+1] = g;
		color[i+2] = b;
	}

	// Remove old memory
	delete colorData;

	colorData = color;

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);
}

void Model::randomizeColor()
{
	GLfloat * color = new GLfloat[faceCount * sizeof(vec3) * 3];

	// Define color data for the object
	for (uint i = 0; i < faceCount * sizeof(vec3); i+=3)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		color[i  ] = r;
		color[i+1] = g;
		color[i+2] = b;
	}

	// Remove old memory
	delete colorData;

	colorData = color;

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);
}

uint Model::getFaceCount()
{
	return faceCount;
}

btRigidBody * Model::getRigidBody()
{
	return rigidBody;
}

float Model::getMass()
{
	return mass;
}

void Model::setVertexData(GLfloat * data)
{
	shapeData = data;
}

void Model::setColorData(GLfloat * data)
{
	colorData = data;
}

void Model::setShader(GLuint shade)
{
	shader = shade;
}

void Model::setTransform(mat4 trans)
{
	transform = trans;
}

void Model::setMass(float m)
{
	mass = m;
}

void Model::setCollisionShape(btCollisionShape * shape)
{
	collisionShape = shape;
}

void Model::initBuffers()
{
	MatrixID      = glGetUniformLocation(shader, "MVP");
    ViewMatrixID  = glGetUniformLocation(shader,   "V");
    ModelMatrixID = glGetUniformLocation(shader,   "M");

	glGenBuffers(1, &verticies);
	glBindBuffer(GL_ARRAY_BUFFER, verticies);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, shapeData, GL_STATIC_DRAW);

	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, colorData, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, faceCount * sizeof(vec3) * 3, normalData, GL_STATIC_DRAW);

	setupComplete = true;
}

void Model::draw(Controls * controls)
{
	if (setupComplete)
	{
		mat4 viewMatrix = controls->getViewMatrix();

		mat4 modelMatrix = transform;
		
		mat4 projectionMatrix = controls->getProjectionMatrix();

		mat4 MVP = projectionMatrix * viewMatrix * transform;

		glUseProgram(shader);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, verticies); 
		glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,  0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colors);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
 		glBindBuffer(GL_ARRAY_BUFFER, normals);
 		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(vec3)*faceCount*3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
}

void Model::printNormals()
{
	
}