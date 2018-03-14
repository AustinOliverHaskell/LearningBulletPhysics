// Plane
#pragma once

#include "glHeader.h"
#include "shapeData.h"

using namespace glm;

class Plane
{
	public:
		Plane();
		~Plane();

		int getIndex();
		void draw();
		mat4 getMatrix();
		void translate();

	private:
		int index;
		mat4   model;
		GLuint verticies;
		GLuint colors;
};