#include "./h/glHeader.h"
#include "./h/Cube.h"
#include "./h/Plane.h"
#include "./h/Controls.h"
#include "./h/defs.h"
#include "./h/FileLoader.h"
#include "./h/GraphicDebugger.h"
#include "./h/Model.h"
#include "./h/World.h"

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
    dynamicsWorld          = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

}
World::~World()
{
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		delete *it;
	}

	delete broadphase;
	delete collisionConfiguration;
	delete dispatcher;
	delete solver;
	delete dynamicsWorld;
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

void World::render()
{
	// Clear the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	controls->computeMatrices();

	// Use our shader
	// 
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	if (glfwGetKey( window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		debugger->showFPS();
	}


	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		(*it)->draw(controls);
	}

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void World::addModel(Model* m)
{
	objects.push_back(m);
	dynamicsWorld->addRigidBody(m->getRigidBody());
}

GLFWwindow* World::getWindow()
{
	return window;
}		