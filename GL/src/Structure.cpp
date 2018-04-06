#include "./h/defs.h"
#include "./h/Structure.h"
#include "./h/PointCloud.h"
#include "./h/FileLoader.h"
#include "./h/World.h"
#include "./h/btFractureBody.h"
#include "./h/Model.h"

class btFractureBody;

#include <iostream>
#include <vector>

using namespace std;

#define FACES_PER_SEGMENT 4
#define TRIANGLE_SIZE 3
#define SEGMENT_MASS 1.0f
#define SCALING 10.0f

Structure::Structure(string path, GLuint shader, World * world)
{
	models = new vector<Model *>();
	clouds = new vector<PointCloud>();

	motionState = new btDefaultMotionState();
	w = world;

	// Load Entire Model
	FileLoader * f = new FileLoader();

	btTransform t;

	if (!f->openFile(path, false))
	{
		cerr << "Error opening file: " + path << endl;
	}
	
	uint size = f->getVertexCount();

	cout << "MAX:" << size/9 << endl;

	GLfloat * shapeData  = f->getObjectData();
	GLfloat * normalData = f->getNormals();

	for (uint i = 0; i < size; i+=FACES_PER_SEGMENT*TRIANGLE_SIZE*3)
	{
		PointCloud temp;
		t.setIdentity();

		for (int p = 0; p < FACES_PER_SEGMENT*TRIANGLE_SIZE*3; p+=3)
		{
			temp.addVertex(shapeData[p+i  ]);
			temp.addVertex(shapeData[p+i+1]);
			temp.addVertex(shapeData[p+i+2]);

			temp.addNormal(normalData[p+i  ]);
			temp.addNormal(normalData[p+i+1]);
			temp.addNormal(normalData[p+i+2]);

			// This value will get rewritten later
			temp.addColor(0.0f, 0.0f, 0.0f);
		}

		clouds->push_back(temp);

		Model * section = new Model(temp, shader, world);

		section->randomizeColor();
		section->setMass(SEGMENT_MASS);
		section->setFriction(10.0f);
		section->setRollingFriction(5.0f);
		section->setRestitution(1.0f);

		section->setScale(vec3(SCALING, SCALING, SCALING));
		section->calcTriangleCollisionMesh();

		section->setPosition(vec3(0, SCALING, 0));

		section->setType("Fragment");

		section->configureRigidBody();
		section->getRigidBody()->setUserPointer((void*)i);
		section->getRigidBody()->setUserIndex(i/9);

		section->getCollisionShape()->setUserIndex(i/9);
		section->getCollisionShape()->setUserPointer(section);

		models->push_back(section);

		world->addModel(section);
		//shape->addChildShape(t, section->getCollisionShape());
	}

	//models->at(0)->setMass(0);
	//models->at(0)->configureRigidBody();

	/*shape->createAabbTreeFromChildren();

	
	t.setIdentity();

	float mass = size;

	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	// Size * 4 is the total mass
	btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, inertia );
	rigidBody = new btFractureBody(info, world->getPhysicsWorld());

	rigidBody->setFriction(0.5f);
	rigidBody->setRollingFriction(1.0f);*/
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
		(*it)->draw(controls);
		//(*it)->transformDraw(controls, trans);
	}

	/*for (uint i = 0; i < models->size(); i++)
	{
		models->at(i)->transformDraw(controls, shape->getChildTransform(i));
	}*/


}

vector<Model*> * Structure::getModels()
{
	return models;
}

void Structure::breakStructure()
{
	for (auto it = models->begin(); it != models->end(); it++)
	{
		((btFractureBody*)(*it)->getRigidBody())->setStrength(0.01f);
		((btFractureBody*)(*it)->getRigidBody())->recomputeConnectivity(w->getDynamicsWorld());
	}

	for (auto it = models->begin(); it != models->end(); it++)
	{
		float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		(*it)->getRigidBody()->applyCentralImpulse(btVector3(x * 100, y * 100, z * 100));
	}	
}