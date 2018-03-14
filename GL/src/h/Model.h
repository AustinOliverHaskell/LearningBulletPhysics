#pragma once

#include "glHeader.h"
#include "Controls.h"

#include <btBulletDynamicsCommon.h>
#include <iostream>

using namespace glm;


/**
	This class just handles the model transformations and also holds its
	 own model data. Knows which shader to use on itslef
*/
class Model
{
	public:
		Model(Model &m);
		Model(std::string path, GLuint shade, bool affectedByPhysics, bool tessalate = false);

		~Model();

		// ----- Setters -----
		void setVertexData(GLfloat * data);
		void setColorData(GLfloat * data);
		// -------------------

		// ----- Getters -----
		GLfloat * getShapeData();
		GLfloat * getColorData();
		uint getFaceCount();

		btRigidBody * getRigidBody();
		float getMass();
		// -------------------

		// ----- Setters -----
		void setColor(float r, float g, float b);
		void randomizeColor();
		void setTransform(mat4 trans);
		void setShader(GLuint shade);


		void setCollisionShape(btCollisionShape * shape);
		void setMass(float m);
		// -------------------

		// ----- Draw -----
		void draw(Controls * controls);
		void initBuffers();
		// ----------------

		// ----- Other -----
		std::string toString();
		void printNormals();
		// -----------------

	private:
		GLfloat * shapeData;
		GLfloat * colorData;
		GLfloat * normalData;

		GLuint verticies;
		GLuint colors;
		GLuint normals;

		mat4 transform;

		GLuint MatrixID;
		GLuint ViewMatrixID;
		GLuint ModelMatrixID;

		GLuint shader;		

		bool setupComplete;

		uint faceCount;

		// ----- Physics Variables -----
		
		// Defaults to a Cube (1, 1, 1)
		btCollisionShape * collisionShape;

		// Rigid Body
		btRigidBody * rigidBody;

		// Motion State
		btDefaultMotionState* motionState;

		float mass;
		// -----------------------------
};