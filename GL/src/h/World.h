#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "Model.h"
#include "glHeader.h"
#include "defs.h"
#include "Controls.h"
#include "GraphicDebugger.h"
#include "c_DebugDraw.h"
#include "Structure.h"
#include "btFractureDynamicsWorld.h"

class Structure;
class Model;
class Controls;
class btFractureDynamicsWorld;

class World
{
	public:
		World();
		~World();

		void step();
		void render();

		void addModel(Model* m);
		void addStructure(Structure * structure);

		btDynamicsWorld * getPhysicsWorld() {return (btDynamicsWorld*)dynamicsWorld;};
		void calcGlue();


		void setLight(GLuint light);
		void setLightPos(vec3 pos);
		void setBackgroundColor(float r, float g, float b);

		GLFWwindow* getWindow();

		btDynamicsWorld * getDynamicsWorld();


	private:

		// ----- OpenGL -----
		std::vector <Model*> objects;
		std::vector <Structure*> structures;

		GLFWwindow* window;

		Controls * controls;
		GraphicDebugger * debugger;

		GLuint VertexArrayID;

		vec3 lightPos;
		GLuint LightID;

		GLuint defaultShader;
		// ------------------

		// ----- Physics -----
		
		// The broadphase is the algorithm that does inital collision calculations
        btBroadphaseInterface * broadphase;

        btDefaultCollisionConfiguration * collisionConfiguration;
        btCollisionDispatcher * dispatcher;


        btSequentialImpulseConstraintSolver *    solver;
        btFractureDynamicsWorld *                dynamicsWorld;
        // -------------------
        
        // ----- Debug -----
        BulletDebugDrawer_OpenGL * debugdrawer;
        bool drawWithDebugging;
        // -----------------
};	