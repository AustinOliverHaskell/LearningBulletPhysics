#pragma once

#include "glHeader.h"
#include "defs.h"

#include <vector>

using namespace std;

class PointCloud
{
	public:
		PointCloud();
		PointCloud(const PointCloud& other);
		~PointCloud();

		// ----- Getters -----

		// This function and the ones following are written to return a copy of 
		//  the data so that there are no memory leaks. The calling class/function
		//  is responsable for clearing out the data returned. The internal vectors
		//  are cleared upon deletion of this object. 

		GLfloat * getVertexData() const;
		GLfloat * getColorData() const;
		GLfloat * getNormalData() const;

		uint getVertexCount() const; 
		uint getColorCount() const;
		uint getNormalCount() const;
		// -------------------
		
		// ----- Setters -----
		void addVertex(GLfloat d);
		void addNormal(GLfloat d);
		void addColor(GLfloat r, GLfloat g, GLfloat b);
		// -------------------
		
		// ----- Other -----
		void clearVertexData();
		void clearColorData();
		void clearNormalData();

		glm::vec3 calcCenter();
		// -----------------

	private:
		vector<GLfloat> * vertexData;
		vector<GLfloat> * colorData; 
		vector<GLfloat> * normalData;
};