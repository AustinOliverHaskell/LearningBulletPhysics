#include "./h/glHeader.h"
#include "./h/Controls.h"
#include "./h/defs.h"
#include "./h/FileLoader.h"
#include "./h/GraphicDebugger.h"
#include "./h/Model.h"

using namespace glm;

Graphics::Graphics(std::string name)
{
	windowName = name;

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
}

Graphics::~Graphics()
{
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

void Graphics::openWindow()
{
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, windowName.c_str(), NULL, NULL);

	// Check to make sure that the window handle was created succesfully
	if( window == NULL )
	{
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
}

void Graphics::setupGL()
{
	Controls * controls = new Controls(window);

	// White background
	glClearColor(0.4f, 0.4f, 0.6f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	// Compile shaders
	GLuint shaders = loadShaders("./shaders/lightShader.vertexshader", "./shaders/lightShader.fragmentshader");
	GLuint solidShader = loadShaders("./shaders/SimpleVertexShader.vertexshader", "./shaders/SimpleFragmentShader.fragmentshader");


	// Get a handle for our "MVP" uniform
	// MVP = Model, View, and Projection
	GLuint MatrixID      = glGetUniformLocation(shaders, "MVP");
	GLuint ViewMatrixID  = glGetUniformLocation(shaders,   "V");
	GLuint ModelMatrixID = glGetUniformLocation(shaders,   "M");

	GLuint colorID 			  = glGetUniformLocation(solidShader, "MVP");
	GLuint colorViewMatrixID  = glGetUniformLocation(solidShader,   "V");
	GLuint colorModelMatrixID = glGetUniformLocation(solidShader,   "M");


	mat4 m1 = mat4(1);
	//m1 = rotate(mat4(), 90.0f, vec3(0.0f, 0.0f, 0.0f));
	//m1 = translate(m1, vec3(-5.0f, -5.0f, -5.0f));
	//m1 = scale(mat4(), vec3(0.25f, 0.25f, 0.25f)); 

	mat4 m2 = translate(mat4(), vec3(-2.0f, -1.01f, -2.0f));
	mat4 m3 = translate(mat4(), vec3(8.0f, 0.0f, 8.0f));
	//Model3 = translate(Model3, vec3(0.0f, -1.0f, 0.0f));
	// Our ModelViewProjection : multiplication of our 3 matrices

	// Could wrap this in some kind of "Object Manager", but thats not needed for this assignment
	Model * model = new Model(shaders, "./obj/Object.obj");
	GraphicDebugger * debugger = new GraphicDebugger();
	Model * sphere = new Model(solidShader, "./obj/sphere.obj");
	//Model * noNormalCube = new Model(shaders, "./obj/no_normals_cube.obj", true);
	Model * lightSource = new Model(solidShader, "./obj/sphere.obj");
	//Model * custom = new Model(solidShader, "./obj/random_multi_layer_test.obj", true);


	model ->initBuffers(MatrixID, ViewMatrixID, ModelMatrixID);
	sphere->initBuffers(MatrixID, ViewMatrixID, ModelMatrixID);

	// This object is for testing the normal calculations
	//noNormalCube->initBuffers(MatrixID, ViewMatrixID, ModelMatrixID);

	lightSource->initBuffers(colorID, colorViewMatrixID, colorModelMatrixID);
	//custom->initBuffers(MatrixID, ViewMatrixID, ModelMatrixID);

	GLuint LightID = glGetUniformLocation(shaders, "LightPosition_worldspace");

	vec3 lightPos = vec3(3, 3, 3);
	mat4 lightMat = translate(mat4(), lightPos);
	lightMat = scale(lightMat, vec3(0.5f, 0.5f, 0.5f));

	sphere->setColor(0.5f, 0.5f, 1.0f);

	do {


		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		controls->computeMatrices();

		// Use our shader
		glUseProgram(shaders);

		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		if (glfwGetKey( window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			debugger->showFPS();
		}

		model->draw(controls, m1);
		//sphere->draw(controls, m2);

		//noNormalCube->draw(controls, m3);
		//lightSource->draw(controls, lightMat);
		//custom->draw(controls, m3);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );


	// Cleanup, each of these objects knows how to clear its own buffers
	//delete noNormalCube;
	delete sphere;
	delete model;
	delete debugger;

	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(shaders);
}



