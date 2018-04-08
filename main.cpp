#include <iostream>
#include <cstdlib>
#include <cmath>
#include <getopt.h>

#include "./GL/src/h/World.h"
#include "./GL/src/h/Model.h"
#include "./GL/src/h/defs.h"
#include "./GL/src/h/Structure.h"
#include "./GL//src/h/Constants.h"

using namespace std;

int main (int argc, char * argv[])
{
    // Defaults
    string mode = "CONCISE";
    string path = "./GL/src/obj/Object.obj";

    int c;

    while ((c = getopt(argc, argv, "hf:v")) != -1)
    {
        switch (c)
        {
            case 'h':
                cerr << argv[0] << "options:" << endl;
                cerr << "  -f Filename to simulate. " << endl;
                cerr << "  -v Enable verbose mode. " << endl;
                exit(0);

            case 'f':
                path = optarg;
                break;

            case 'v':
                mode = "VERBOSE";
                break;
        }
    }

    srand (static_cast <unsigned> (time(0)));

    std::cout << "Thesis" << std::endl;

    // Create the world
    World * world = new World();

    // Load in the shaders that were going to use
    GLuint shader      = loadShaders("./GL/src/shaders/lightShader.vertexshader", "./GL/src/shaders/lightShader.fragmentshader");
    GLuint solidShader = loadShaders("./GL/src/shaders/SimpleVertexShader.vertexshader", "./GL/src/shaders/SimpleFragmentShader.fragmentshader");

    // Create the memory for our singular light
    GLuint LightID = glGetUniformLocation(shader, "LightPosition_worldspace");

    vec3 lightPosition = vec3(5.0f, 5.0f, 5.0f);

    // Let there be light
    world->setLight(LightID);
    world->setLightPos(lightPosition);

    // Models in our scene, the plane needs to have its normals calculated
    Model * object = new Model("./GL/src/obj/Object.obj", solidShader, world);
    Model * sphere = new Model("./GL/src/obj/sphere.obj", solidShader, world);
    Model * plane  = new Model("./GL/src/obj/plane.obj",  solidShader, world, true);
    Model * light  = new Model("./GL/src/obj/cube.obj", solidShader, world);
    Structure * s  = new Structure(path, solidShader, world);  

    // Initilize our buffers
    object->initBuffers();
    plane ->initBuffers();
    light ->initBuffers();
    sphere->initBuffers();

    // Set the plane to a different color
    object->randomizeColor();
    sphere->setColor(PARTICLE_R, PARTICLE_G, PARTICLE_B);
    plane ->setColor(1.0f, 0.5f, 0.25f);
    light ->setColor(1.0f, 1.0f, 1.0f);

    // Physical Attributes
    object->setMass(50);
    sphere->setMass(WEIGHT);

    object->setFriction(0.1f);
    sphere->setFriction(0.1f);
    plane ->setFriction(2.0f);

    object->setRollingFriction(0.01f);
    sphere->setRollingFriction(0.05f);

    object->setRestitution(0.5);
    sphere->setRestitution(0.5);
    plane ->setRestitution(0.5);

    plane ->setCollisionShape(new btStaticPlaneShape(btVector3(0, 1, 0), 0));
    sphere->setCollisionShape(new btSphereShape(1));

    object->setPosition(vec3(0.0f, 10.0f, 0.0f));
    sphere->setPosition(vec3(0.0f, 12.0f, 0.5f));
    plane ->setPosition(vec3(0.0f, 0.0f, 0.0f));
    light ->setPosition(lightPosition);

    plane ->setScale(vec3(60.0f, 0.0f, 60.0f));
    sphere->setScale(vec3(PARTICLE_SCALING, PARTICLE_SCALING, PARTICLE_SCALING));
    object->setScale(vec3(2.0f, 2.0f, 2.0f));

    sphere->setType("Raindrop");


    // This one needs a special mesh, the meshes are slower but model
    //  the object much better
    object->calcTriangleCollisionMesh();

    // Add them to the scene
    // Structures add themselfs to the world from their constructor
    world->addModel(plane);

    // Set Backgrond to black
    world->setBackgroundColor(0.0f, 0.0f, 0.0f);
    world->calcGlue();

    cout << " --------------- Now Rendering ---------------" << endl;

    // Render
    while(glfwGetKey(world->getWindow(), GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(world->getWindow()) == 0 )
    {
        for (int i = 0; i < PARTICLE_COUNT; i++)
        {
            float x = -40.0f;
            float y = 2 + rand() % RANGE;
            float z = rand() % RANGE - (rand() % RANGE);

            Model * temp = new Model(sphere, world);
            temp->setPosition(vec3(x, y, z));
            temp->configureRigidBody();
            temp->getRigidBody()->applyCentralImpulse(btVector3(WIND_SPEED_X, WIND_SPEED_Y, WIND_SPEED_Z));
            world->addModel(temp);
        }
        if (mode == "VERBOSE")
        {
            world->render();
        }
        else
        {
            world->step();
        }
    }

    // The world object will handle the destruction of all models in its space
    delete world;

    return 0;
}