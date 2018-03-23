#include "./h/defs.h"
#include "./h/Structure.h"
#include "./h/Model.h"
#include "./h/PointCloud.h"
#include "./h/FileLoader.h"
#include "./h/World.h"
#include "./h/btFractureBody.h"

#include <iostream>
#include <vector>

using namespace std;

#define FACES_PER_SEGMENT 4
#define TRIANGLE_SIZE 3

Structure::Structure(string path, GLuint shader)
{
	models = new vector<Model *>();
	clouds = new vector<PointCloud>();

	shape = new btCompoundShape();
	motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.0f, 5.0f, 0.0f)));

	// Load Entire Model
	FileLoader * f = new FileLoader();

	if (!f->openFile(path, false))
	{
		cerr << "Error opening file: " + path << endl;
	}
	
	uint size = f->getVertexCount();

	GLfloat * shapeData  = f->getObjectData();
	GLfloat * normalData = f->getNormals();

	// TODO: Make this load the entire structure
	for (uint i = 0; i < size; i+=FACES_PER_SEGMENT*TRIANGLE_SIZE*3)
	{
		PointCloud temp;

		for (int p = 0; p < FACES_PER_SEGMENT*TRIANGLE_SIZE*3; p+=3)
		{
			temp.addVertex(shapeData[p+i  ]);
			temp.addVertex(shapeData[p+i+1]);
			temp.addVertex(shapeData[p+i+2]);

			temp.addNormal(normalData[p+i  ]);
			temp.addNormal(normalData[p+i+1]);
			temp.addNormal(normalData[p+i+2]);

			temp.addColor(0.0f, 0.0f, 0.0f);

			//cout << " -- " << endl;
			//cout << shapeData[p+i  ] << endl;
			//cout << shapeData[p+i+1] << endl;
			//cout << shapeData[p+i+2] << endl;
			//cout << " -- " << endl;
		}

		//cout << " ----- " << endl;

		//cout << "Created Split #" << i / (FACES_PER_SEGMENT*TRIANGLE_SIZE) << endl;

		clouds->push_back(temp);

		Model * section = new Model(temp, shader);

		section->randomizeColor();
		section->setMass(1.0f);
		section->setFriction(1.0f);
		section->setRollingFriction(0.1f);
		section->setScale(vec3(2.0f, 2.0f, 2.0f));
		section->setRestitution(1.0f);

		section->setPosition(temp.calcCenter());

		section->configureRigidBody();

		models->push_back(section);

		shape->addChildShape(btTransform(), section->getCollisionShape());
	}

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(size*4, inertia);
	// Size * 4 is the total mass
	btRigidBody::btRigidBodyConstructionInfo info(size * 4, motionState, shape, inertia );
	rigidBody = new btRigidBody(info);

	rigidBody->setFriction(1.0f);
	rigidBody->setRestitution(1.0f);
}

Structure::~Structure()
{
	for (auto it = models->begin(); it != models->end(); it++)
	{
		delete *it;
	}

	delete models;
	delete clouds;
	delete shape;
	delete motionState;
	delete rigidBody;
}

void Structure::render(Controls * controls)
{
	btTransform trans;
    motionState->getWorldTransform(trans);

	for (auto it = models->begin(); it != models->end(); it++)
	{
		(*it)->transformDraw(controls, trans);
	}
}

vector<Model*> * Structure::getModels()
{
	return models;
}