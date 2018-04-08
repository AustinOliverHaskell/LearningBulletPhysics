#include "./h/glHeader.h"
#include "./h/Cube.h"
#include "./h/Plane.h"
#include "./h/Controls.h"
#include "./h/defs.h"
#include "./h/FileLoader.h"
#include "./h/GraphicDebugger.h"
#include "./h/Model.h"
#include "./h/World.h"
#include "./h/c_DebugDraw.h"
#include "./h/Structure.h"
#include "./h/btFractureDynamicsWorld.h"

#include <btBulletDynamicsCommon.h>

using namespace glm;

World::World()
{
	// Initialise GLFW
	if(!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// These two lines make sure that the current API client is a compatable version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed

	// Specifies which openGl profile to create context for openGL, openGl 3 in this case
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Rendering", NULL, NULL);

	// Check to make sure that the window handle was created succesfully
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	controls = new Controls(window);

	glClearColor(0.4f, 0.4f, 0.6f, 0.0f);
	lightPos = vec3(10, 10, 10);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	debugger = new GraphicDebugger();

	// Start of Physics Init
	broadphase             = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher             = new btCollisionDispatcher(collisionConfiguration);
    solver                 = new btSequentialImpulseConstraintSolver();
    dynamicsWorld          = new btFractureDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

    debugdrawer  = new BulletDebugDrawer_OpenGL();
    
	debugdrawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe);

	dynamicsWorld->setDebugDrawer(debugdrawer);

	// This is a quick fix for the debug drawer
	defaultShader = loadShaders("./GL/src/shaders/SimpleVertexShader.vertexshader", "./GL/src/shaders/SimpleFragmentShader.fragmentshader");

	debugdrawer->init(controls, defaultShader);

	dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	dynamicsWorld->setFractureMode(false);

	drawWithDebugging = false;
}
World::~World()
{
	cout << "Items in world at destruction: "<< objects.size() + structures.size() << endl;
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		dynamicsWorld->removeRigidBody((*it)->getRigidBody());
		delete *it;
	}

	delete broadphase;
	delete collisionConfiguration;
	delete dispatcher;
	delete solver;
	// TODO: figure out why this line is causing a segfault
	//delete dynamicsWorld;
}

void World::setLight(GLuint light)
{
	LightID = light; 	
}

void World::setLightPos(vec3 pos)
{
	lightPos = pos;
}

void World::setBackgroundColor(float r, float g, float b)
{
	glClearColor(r, g, b, 0.0f);
}

void World::calcGlue()
{
    dynamicsWorld->stepSimulation(1.0f / 60.0f);
    dynamicsWorld->glueCallback();
    dynamicsWorld->setFractureMode(true);
    std::cout << "Fracture mode: ON" << std::endl;
}

void World::step()
{
	dynamicsWorld->stepSimulation(1.0f / 60.0f);
}

void World::render()
{
	controls->computeMatrices();

	dynamicsWorld->stepSimulation(1.0f / 60.0f);
	//dynamicsWorld->solveConstraints(dynamicsWorld->getSolverInfo());

	// Clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	// Use our shader
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	if (glfwGetKey( window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		debugger->showFPS();
	}

	// ----- Turn on and off fracture mode -----
	static int oldState = GLFW_RELEASE;
	int newState = glfwGetKey(window, GLFW_KEY_B);

	if (newState == GLFW_RELEASE && oldState == GLFW_PRESS)
	{
		dynamicsWorld->setFractureMode(!dynamicsWorld->getFractureMode());
		std::cout << "Fracture Mode: " << (dynamicsWorld->getFractureMode() ? "ON" : "OFF") << std::endl;
	}
	oldState = newState;
	// -----------------------------------------
	

	// ----- Apply an impulse to an object -----
	static int oldStateG = GLFW_RELEASE;
	int newStateG = glfwGetKey(window, GLFW_KEY_G);

	if (newStateG == GLFW_RELEASE && oldStateG == GLFW_PRESS)
	{
		dynamicsWorld->setFractureMode(false);
		btRigidBody * hit = controls->grabObject(dynamicsWorld);

		// If some internal states are off then this can return null, not a problem
		//  just needs to be checked
		if (hit != nullptr)
		{
			hit->forceActivationState(ACTIVE_TAG);
			hit->applyCentralImpulse(btVector3(0.0f, 200.0f, 0.0f));

			//btTransform trans;

			//hit->getMotionState()->getWorldTransform(trans);

			//std::cout << "X" << trans.getOrigin().getX();
		}
		dynamicsWorld->setFractureMode(true);
	}
	oldStateG = newStateG;
	// -----------------------------------------
	

	// ----- Turn on and off debug drawing -----
	static int oldStateH = GLFW_RELEASE;
	int newStateH = glfwGetKey(window, GLFW_KEY_H);

	if (newStateH == GLFW_RELEASE && oldStateH == GLFW_PRESS)
	{
		drawWithDebugging = !drawWithDebugging;
	}
	oldStateH = newStateH;

	if (drawWithDebugging)
	{
		dynamicsWorld->debugDrawWorld();
		debugdrawer->draw();
	}
	// -----------------------------------------
	
	dynamicsWorld->printCompounds(controls);	

	// ----- Render objects (WIP) -----
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it)->getType() != "Fragment")
		{
			(*it)->draw(controls);
		}
		if (((*it)->getType() == "Raindrop") && ((*it)->getPosition().y <= 1))
		{
			dynamicsWorld->removeRigidBody((*it)->getRigidBody());
			objects.erase(it);
			continue;
		}
	}

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
	// --------------------------------
}

void World::addModel(Model* m)
{
	objects.push_back(m);
	dynamicsWorld->addRigidBody(m->getRigidBody());
}

void World::addStructure(Structure * structure)
{
	structures.push_back(structure);
	dynamicsWorld->addRigidBody(structure->getRigidBody());
}

btDynamicsWorld * World::getDynamicsWorld()
{
	return dynamicsWorld;
}

GLFWwindow* World::getWindow()
{
	return window;
}		