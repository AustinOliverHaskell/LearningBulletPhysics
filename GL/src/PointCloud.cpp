#include <iostream>
#include <vector>

#include "./h/glHeader.h"
#include "./h/PointCloud.h"
#include "./h/defs.h"

using namespace std;

PointCloud::PointCloud()
{
	vertexData = new vector<GLfloat>();
	colorData  = new vector<GLfloat>();
	normalData = new vector<GLfloat>();
}

PointCloud::PointCloud(const PointCloud& other)
{
	vertexData = new vector<GLfloat>();
	colorData  = new vector<GLfloat>();
	normalData = new vector<GLfloat>();

	GLfloat * vtemp = other.getVertexData();
	GLfloat * ctemp = other.getColorData();
	GLfloat * ntemp = other.getNormalData();

	for (uint i = 0; i < other.getVertexCount(); i++)
	{
		vertexData->push_back(vtemp[i]);
	}
	for (uint i = 0; i < other.getColorCount(); i++)
	{
		colorData->push_back(ctemp[i]);
	}
	for (uint i = 0; i < other.getNormalCount(); i++)
	{
		normalData->push_back(ntemp[i]);
	}
}

PointCloud::~PointCloud()
{
	delete vertexData;
	delete colorData;
	delete normalData;
}

// ----- Getters -----
GLfloat * PointCloud::getVertexData() const
{
	GLfloat * retVal = new GLfloat[vertexData->size()];

	for (uint i = 0; i < vertexData->size(); i++)
	{
		retVal[i] = (*vertexData)[i];
	}

	return retVal;
}

GLfloat * PointCloud::getColorData() const
{
	GLfloat * retVal = new GLfloat[colorData->size()];

	for (uint i = 0; i < colorData->size(); i++)
	{
		retVal[i] = (*colorData)[i];
	}

	return retVal;
}

GLfloat * PointCloud::getNormalData() const
{
	GLfloat * retVal = new GLfloat[normalData->size()];

	for (uint i = 0; i < normalData->size(); i++)
	{
		retVal[i] = (*normalData)[i];
	}

	return retVal;
}

uint PointCloud::getVertexCount() const
{
	return vertexData->size();
}

uint PointCloud::getColorCount() const
{
	return colorData->size();
}

uint PointCloud::getNormalCount() const
{
	return normalData->size();
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

glm::vec3 PointCloud::calcCenter()
{
	glm::vec3 retVal(0, 0, 0);

	for (uint i = 0; i < vertexData->size(); i+=3)
	{
		retVal.x += (*vertexData)[i];
		retVal.y += (*vertexData)[i+1];
		retVal.z += (*vertexData)[i+2];
	}

	uint numberOfPoints = vertexData->size() / 3;

	retVal.x = retVal.x / numberOfPoints;
	retVal.y = retVal.y / numberOfPoints;
	retVal.z = retVal.z / numberOfPoints;

	return retVal;
}
// -----------------