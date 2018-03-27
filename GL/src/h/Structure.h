#pragma once

#include "Model.h"
#include "glHeader.h"
#include "PointCloud.h"
#include "World.h"
#include "btFractureBody.h"

class World;
class Model;

class Structure
{
	public:
		Structure(string path, GLuint shader, World * world);
		~Structure();

		btRigidBody * getRigidBody() {return rigidBody;}
		void render(Controls * controls);

		// For Debugging
		vector<Model*> * getModels();

		void breakStructure();

	private:
		vector <Model *> * models;
		vector <PointCloud> * clouds;

		btCompoundShape * shape;
		btMotionState * motionState;
		btFractureBody * rigidBody;

		World * w;
};