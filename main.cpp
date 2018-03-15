#include <iostream>
#include <cstdlib>

#include "./GL/src/h/World.h"
#include "./GL/src/h/Model.h"
#include "./GL/src/h/defs.h"

int main (int argc, char * argv[])
{
    srand (static_cast <unsigned> (time(0)));

    std::cout << "Thesis" << std::endl;

    // Create the world
    World * world = new World();

    // Load in the shaders that were going to use
    GLuint shader = loadShaders("./GL/src/shaders/lightShader.vertexshader", "./GL/src/shaders/lightShader.fragmentshader");
    GLuint solidShader = loadShaders("./GL/src/shaders/SimpleVertexShader.vertexshader", "./GL/src/shaders/SimpleFragmentShader.fragmentshader");

    // Create the memory for our singular light
    GLuint LightID = glGetUniformLocation(shader, "LightPosition_worldspace");

    vec3 lightPosition = vec3(5.0f, 5.0f, 5.0f);

    // Let there be light
    world->setLight(LightID);
    world->setLightPos(lightPosition);

    // Models in our scene, the plane needs to have its normals calculated
    Model * object = new Model("./GL/src/obj/Object.obj", solidShader, true);
    Model * sphere = new Model("./GL/src/obj/sphere.obj", shader, true);
    Model * cube   = new Model("./GL/src/obj/cube.obj", shader, true);
    Model * cube2  = new Model("./GL/src/obj/cube.obj", shader, true);
    Model * plane  = new Model("./GL/src/obj/plane.obj",  solidShader, false, true);
    Model * light  = new Model("./GL/src/obj/cube.obj", solidShader, false);

    // Initilize our buffers
    object->initBuffers();
    plane->initBuffers();
    light->initBuffers();
    sphere->initBuffers();
    cube->initBuffers();
    cube2->initBuffers();

    // Set the plane to a different color
    object->randomizeColor();
    sphere->setColor(1.0f, 0.0f, 0.0f);
    cube->randomizeColor();
    cube2->randomizeColor();
    plane->setColor(1.0f, 0.5f, 0.25f);
    light->setColor(1.0f, 1.0f, 1.0f);

    object->setMass(1);
    sphere->setMass(1);
    cube->setMass(0.5);
    cube2->setMass(0.5);

    object->setFriction(0.1f);
    sphere->setFriction(0.1f);
    cube->setFriction(0.1);
    cube2->setFriction(0.1);
    plane->setFriction(1.0f);

    object->setRollingFriction(0.01f);
    sphere->setRollingFriction(0.01f);

    object->setRestitution(1.5);
    cube->setRestitution(3);

    plane->setCollisionShape(new btStaticPlaneShape(btVector3(0, 1, 0), 0));
    object->setCollisionShape(new btSphereShape(1));
    sphere->setCollisionShape(new btSphereShape(1));
    cube2->setCollisionShape(new btBoxShape(btVector3(2, 2, 2)));
    // Cube will just use the defaults

    object->setPosition(vec3(0.0f, 10.0f, 0.0f));
    sphere->setPosition(vec3(0.0f, 12.0f, 0.5f));
    plane->setPosition(vec3(0.0f, 0.0f, 0.0f));
    light->setPosition(lightPosition);
    cube->setPosition(vec3(0.0f, 7.0f, -0.5f));
    cube2->setPosition(vec3(0.0f, 6.0f, -1.0f));

    plane->setScale(vec3(30.0f, 0.0f, 30.0f));
    cube2->setScale(vec3(2.0f, 2.0f, 2.0f));

    // Add them to the scene
    world->addModel(object);
    world->addModel(plane);
    world->addModel(light);
    world->addModel(sphere);
    world->addModel(cube);
    world->addModel(cube2);

    // Set Backgrond to black
    world->setBackgroundColor(0.0f, 0.0f, 0.0f);

    // Render
    while(glfwGetKey(world->getWindow(), GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(world->getWindow()) == 0 )
    {
        world->render();
    }

    // The world object will handle the destruction of all models in its space
    delete world;

    return 0;
}