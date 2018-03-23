#pragma once

#include "Model.h"
#include "glHeader.h"
#include "PointCloud.h"
#include "World.h"

class Structure
{
	public:
		Structure(string path, GLuint shader);
		~Structure();

		btRigidBody * getRigidBody() {return rigidBody;}
		void render(Controls * controls);

		// For Debugging
		vector<Model*> * getModels();

	private:
		vector <Model *> * models;
		vector <PointCloud> * clouds;

		btCompoundShape * shape;
		btMotionState * motionState;
		btRigidBody * rigidBody;
};