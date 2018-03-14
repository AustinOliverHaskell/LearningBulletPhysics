#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>


#include "Model.h"
#include "glHeader.h"
#include "defs.h"
#include "Controls.h"
#include "GraphicDebugger.h"

class World
{
	public:
		World();
		~World();

		void render();

		void addModel(Model* m);

		void setLight(GLuint light);
		void setLightPos(vec3 pos);
		void setBackgroundColor(float r, float g, float b);

		GLFWwindow* getWindow();

	private:

		// ----- OpenGL -----
		std::vector <Model*> objects;
		GLFWwindow* window;

		Controls * controls;
		GraphicDebugger * debugger;

		GLuint VertexArrayID;

		vec3 lightPos;
		GLuint LightID;
		// ------------------




		// ----- Physics -----
		
		// The broadphase is the algorithm that does inital collision calculations
        btBroadphaseInterface * broadphase;

        btDefaultCollisionConfiguration * collisionConfiguration;
        btCollisionDispatcher * dispatcher;


        btSequentialImpulseConstraintSolver *    solver;
        btDiscreteDynamicsWorld *                dynamicsWorld;
        // -------------------
};	