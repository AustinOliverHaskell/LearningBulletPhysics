#include <iostream>
#include <vector>

#include "./h/glHeader.h"
#include "./h/PointCloud.h"

using namespace std;

PointCloud::PointCloud()
{
	vertexData = new vector<GLfloat>();
	colorData  = new vector<GLfloat>();
	normalData = new vector<GLfloat>();
}
PointCloud::~PointCloud()
{
	delete vertexData;
	delete colorData;
	delete normalData;
}

// ----- Getters -----
GLfloat * PointCloud::getVertexData()
{
	GLfloat * retVal = new GLfloat[vertexData->size()];

	for (uint i = 0; i < vertexData->size(); i++)
	{
		retVal[i] = (*vertexData)[i];
		cout << retVal[i] << endl;
	}

	return retVal;
}

GLfloat * PointCloud::getColorData()
{
	GLfloat * retVal = new GLfloat[colorData->size()];

	for (uint i = 0; i < colorData->size(); i++)
	{
		retVal[i] = (*colorData)[i];
	}

	return retVal;
}

GLfloat * PointCloud::getNormalData()
{
	GLfloat * retVal = new GLfloat[normalData->size()];

	for (uint i = 0; i < normalData->size(); i++)
	{
		retVal[i] = (*normalData)[i];
	}

	return retVal;
}
// -------------------


// ----- Setters -----
void PointCloud::addVertex(GLfloat d)
{
	vertexData->push_back(d);
}
void PointCloud::addNormal(GLfloat d)
{
	normalData->push_back(d);
}
void PointCloud::addColor(GLfloat r, GLfloat g, GLfloat b)
{
	colorData->push_back(r);
	colorData->push_back(g);
	colorData->push_back(b);

}
// -------------------


// ----- Other -----
void PointCloud::clearVertexData()
{
	vertexData->clear();
}
void PointCloud::clearColorData()
{
	colorData->clear();
}
void PointCloud::clearNormalData()
{
	normalData->clear();
}
// -----------------