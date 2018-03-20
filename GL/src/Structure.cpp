#include "./h/defs.h"
#include "./h/Structure.h"
#include "./h/Model.h"
#include "./h/PointCloud.h"
#include "./h/FileLoader.h"
#include "./h/World.h"

#include <iostream>
#include <vector>

using namespace std;

#define FACES_PER_SEGMENT 4
#define TRIANGLE_SIZE 3

Structure::Structure(string path, GLuint shader)
{
	models = new vector<Model *>();
	clouds = new vector<PointCloud>();
	// Load Entire Model
	FileLoader * f = new FileLoader();

	if (!f->openFile(path, false))
	{
		cerr << "Error opening file: " + path << endl;
	}
	
	uint size = f->getVertexCount();

	GLfloat * shapeData  = f->getObjectData();
	GLfloat * normalData = f->getNormals();

	cout << "Size: " << size << endl;

	for (uint i = 0; i < size; i+=FACES_PER_SEGMENT*TRIANGLE_SIZE)
	{
		PointCloud temp;

		for (int p = 0; p < FACES_PER_SEGMENT*TRIANGLE_SIZE; p++)
		{
			temp.addVertex(shapeData[i]);
			temp.addNormal(normalData[i]);
			temp.addColor(0.0f, 0.0f, 0.0f);
		}

		clouds->push_back(temp);

		Model * section = new Model(temp, shader);

		section->randomizeColor();
		section->setMass(1.0f);
		section->setFriction(0.1f);

		section->initBuffers();

		models->push_back(section);
	}
}

Structure::~Structure()
{
	for (auto it = models->begin(); it != models->end(); it++)
	{
		delete *it;
	}

	delete models;
	delete clouds;
}

void Structure::addToWorld(World * w)
{
	for (auto it = models->begin(); it != models->end(); it++)
	{
		w->addModel(*it);
	}
}
