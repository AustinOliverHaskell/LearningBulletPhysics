#include "./h/FileLoader.h"
#include "./h/defs.h"
#include "./objLoaders/objload.h"
#include "./h/glHeader.h"

#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

FileLoader::FileLoader()
{
	faceCount = 0;
}

FileLoader::~FileLoader()
{
	delete vertexData;
}

GLfloat * FileLoader::getObjectData()
{
	// TODO: These should return copies so that the vector can be 
	//  deleted cleanly.
	return &verticies[0];
}

GLfloat * FileLoader::getNormals()
{
	return &normals[0];
}

uint FileLoader::getFaceCount()
{
	return faceCount;
}

uint FileLoader::getVertexCount()
{
	return verticies.size();
}

uint FileLoader::getNormalCount()
{
	return normals.size();

}

glm::vec3 FileLoader::calcNormal(glm::vec3 one, glm::vec3 two, glm::vec3 three)
{
	glm::vec3 retVal(0, 0, 0);

	one = normalize(one);
	two = normalize(two);
	three = normalize(three);

	glm::vec3 edgeOne(0, 0, 0);
	glm::vec3 edgeTwo(0, 0, 0);

	// Calc Edges
	edgeOne = two - one;
	edgeTwo = three - one;

	edgeOne = normalize(edgeOne);
	edgeTwo = normalize(edgeTwo);

	// Cross Product
	retVal = glm::cross(edgeOne, edgeTwo);

	
	retVal = normalize(retVal);
	

	return retVal;
}

// Wrapper function to make sure that a 0 vector does not
//  get passed in
glm::vec3 FileLoader::normalize(glm::vec3 vec)
{
	glm::vec3 retVal = vec;

	if (vec != glm::vec3(0,0,0))
	{
		retVal = glm::normalize(vec);
	}

	return retVal;
}



bool FileLoader::openFile(string path, bool tesselate)
{
	bool retVal = true;

	cout << "Object :" << path << endl;

	ifstream in;

	in.open(path);

	// Check if the file is open
	retVal = in.is_open();

	if (!retVal)
	{
		return retVal;
	}

	// Load the file
	obj::ObjModel model = obj::parseObjModel(in);

	// Object isnt in triangle form
	if (tesselate)
	{
		// Make sure that the model is in triangle form
		tesselateObjModel(model);
	}

	// Just get the start iterator for the faces map
	auto it = model.faces.cbegin();

	// Second in this context refers to the value of the map
	//  second.first refers to the first item of the contaned tuple
	//  which in this case is a vector
	faceCount = it->second.first.size() / 3;

	// second.first is refering to the underlying vector of facepoints 
	for (auto x = it->second.first.begin(); x != it->second.first.end(); ++x)
	{
		// Get the vertex info and then multiply it by 3 to offset it
		//  around the x y and z points
		int pos = (x->v)*3;

		//cout << pos/3+1 << " - ";

		// Push back each point that is listed in the face description
		verticies.push_back(model.vertex[pos]);
		verticies.push_back(model.vertex[pos+1]);
		verticies.push_back(model.vertex[pos+2]);

		// If the file did not get tessalated, because this is a crash otherwise
		if (!tesselate)
		{
			int norPos = (x->n)*3;

			normals.push_back(model.normal[norPos]);
			normals.push_back(model.normal[norPos+1]);
			normals.push_back(model.normal[norPos+2]);

			if (path == "./obj/cube.obj")
			{
				cout << model.normal[norPos] << " " << model.normal[norPos+1] << " " << model.normal[norPos + 2] << endl;
			}
		}
	}

	if (tesselate)
	{
		// Calculate Normals
		for (uint i = 0; i < verticies.size() - 9; i+=9)
		{
			// TODO: Use the Calc normal function to create normal data
			//  this needs to happen on each pair of points
			glm::vec3 one;
			one.x = verticies[i    ];
			one.y = verticies[i + 1];
			one.z = verticies[i + 2];

			glm::vec3 two;
			two.x = verticies[i + 3];
			two.y = verticies[i + 4];
			two.z = verticies[i + 5];

			glm::vec3 three;
			three.x = verticies[i + 6];
			three.y = verticies[i + 7];
			three.z = verticies[i + 8];

			glm::vec3 point = calcNormal(one, two, three);

			normals.push_back(point.x);
			normals.push_back(point.y);
			normals.push_back(point.z);

			normals.push_back(point.x);
			normals.push_back(point.y);
			normals.push_back(point.z);

			normals.push_back(point.x);
			normals.push_back(point.y);
			normals.push_back(point.z);

			cout << point.x << " " << point.y << " "<< point.z << endl;
			cout << point.x << " " << point.y << " "<< point.z << endl;
			cout << point.x << " " << point.y << " "<< point.z << endl;

		}
	}



	return retVal;
}