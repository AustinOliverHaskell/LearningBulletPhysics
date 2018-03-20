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

		void addToWorld(World * w );

	private:
		vector <Model *> * models;
		vector <PointCloud> * clouds;
};